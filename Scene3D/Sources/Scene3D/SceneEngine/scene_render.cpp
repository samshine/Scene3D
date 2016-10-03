
#include "precomp.h"
#include "scene_render.h"
#include "Scene3D/Performance/scope_timer.h"
#include "Scene3D/Scene/scene_camera_impl.h"
#include "Scene3D/SceneEngine/scene_viewport_impl.h"

using namespace uicore;

SceneRender::SceneRender(SceneEngineImpl *engine) : engine(engine)
{
}

void SceneRender::wait_next_frame_ready(const GraphicContextPtr &gc)
{
	if (frames.empty() || !frames.back()->frame_finished)
		return;

	auto dc = D3DTarget::device_context_handle(gc);
	if (!dc)
		return;

	while (true)
	{
		BOOL data = 0;
		HRESULT result = dc->GetData(frames.back()->frame_finished, &data, sizeof(DWORD), 0);
		if (result == S_OK)
			break;
		else if (FAILED(result))
			throw Exception("D3D11DeviceContext.GetData(D3D11_QUERY_EVENT) failed");
		System::sleep(1); // TBD; is this a good idea?
	}
}

void SceneRender::render(const GraphicContextPtr &render_gc, SceneViewportImpl *new_scene_viewport)
{
	ScopeTimeFunction();

	models_drawn = 0;
	instances_drawn = 0;
	draw_calls = 0;
	triangles_drawn = 0;
	scene_visits = 0;

	scene_viewport = new_scene_viewport;

	if (!scene_viewport->camera())
		return;

	scene = scene_viewport->scene();
	camera = static_cast<SceneCameraImpl*>(scene_viewport->camera().get());

	final_viewport = scene_viewport->_viewport;
	fb_viewport = scene_viewport->_fb_viewport;
	viewport_size = Size((int)std::round(final_viewport.width() / scene_viewport->_viewport_scale), (int)std::round(final_viewport.height() / scene_viewport->_viewport_scale));

	field_of_view = camera->field_of_view();
	Quaternionf inv_orientation = Quaternionf::inverse(scene_viewport->camera()->orientation());
	world_to_eye = inv_orientation.to_matrix() * Mat4f::translate(-scene_viewport->camera()->position());

	gc = render_gc;
	gpu_timer.begin_frame(gc);

	setup_screen_quad(gc);
	setup_passes();

	if (frames.empty())
	{
		frames.push_back(std::make_shared<SceneRenderFrame>());
		frames.push_back(std::make_shared<SceneRenderFrame>());
	}
	else
	{
		frames.push_back(frames.front());
		frames.pop_front();
	}

	frames.front()->setup_pass_buffers(this);

	frames.front()->next_model_instance_buffer = 0;
	frames.front()->next_model_staging_buffer = 0;
	frames.front()->next_model_render_uniforms = 0;
	frames.front()->next_decal_instance_buffer = 0;
	frames.front()->next_decal_staging_buffer = 0;
	frames.front()->next_decal_render_uniforms = 0;

	if (!shadow_maps)
		shadow_maps = std::make_shared<ShadowMaps>(*this);

	for (const auto &pass : passes)
	{
		gpu_timer.begin_time(gc, pass->name());
		pass->run();
		gpu_timer.end_time(gc);

		if (render_gc->shader_language() == shader_glsl)
		{
			try
			{
				OpenGL::check_error();
			}
			catch (const Exception &e)
			{
				throw Exception(string_format("%1: %2", pass->name(), e.message));
			}
		}
	}

	gpu_timer.end_frame(gc);
	gpu_results = gpu_timer.get_results(gc);

	if (!frames.front()->frame_finished && render_gc->shader_language() == shader_hlsl)
	{
		auto device = D3DTarget::device_handle(gc);
		if (device)
		{
			D3D11_QUERY_DESC query_desc;
			query_desc.Query = D3D11_QUERY_EVENT;
			query_desc.MiscFlags = 0;
			HRESULT result = device->CreateQuery(&query_desc, frames.front()->frame_finished.output_variable());
			if (FAILED(result))
				throw Exception("D3D11Device.CreateQuery(D3D11_QUERY_EVENT) failed");
		}
	}

	if (frames.front()->frame_finished)
	{
		auto dc = D3DTarget::device_context_handle(gc);
		dc->End(frames.front()->frame_finished);
	}

	gc = nullptr;
	camera = nullptr;
	scene = nullptr;

	if (render_gc->shader_language() == shader_glsl)
		OpenGL::check_error();
}

void SceneRender::update(const GraphicContextPtr &render_gc, SceneViewportImpl *viewport, float new_time_elapsed)
{
	if (!viewport->camera())
		return;

	scene = viewport->scene();
	camera = static_cast<SceneCameraImpl*>(viewport->camera().get());
	gc = render_gc;
	time_elapsed = new_time_elapsed;

	for (const auto &pass : passes)
		pass->update();

	gc = nullptr;
	camera = nullptr;
	scene = nullptr;
	time_elapsed = 0.0f;
	// To do: update scene object animations here too
}

void SceneRender::setup_screen_quad(const GraphicContextPtr &gc)
{
	if (screen_quad_primitives)
		return;

	Vec4f positions[6] =
	{
		Vec4f(-1.0f, -1.0f, 1.0f, 1.0f),
		Vec4f(1.0f, -1.0f, 1.0f, 1.0f),
		Vec4f(-1.0f,  1.0f, 1.0f, 1.0f),
		Vec4f(1.0f, -1.0f, 1.0f, 1.0f),
		Vec4f(-1.0f,  1.0f, 1.0f, 1.0f),
		Vec4f(1.0f,  1.0f, 1.0f, 1.0f)
	};
	screen_quad_positions = VertexArrayVector<Vec4f>(gc, positions, 6);
	screen_quad_primitives = PrimitivesArray::create(gc);
	screen_quad_primitives->set_attributes(0, screen_quad_positions);

	BlendStateDescription blend_desc;
	blend_desc.enable_blending(false);
	no_blend = gc->create_blend_state(blend_desc);

	RasterizerStateDescription rasterizer_desc;
	rasterizer_desc.set_culled(false);
	no_cull_rasterizer = gc->create_rasterizer_state(rasterizer_desc);
}

void SceneRender::setup_passes()
{
	if (!passes.empty())
		return;

	bool use_compute_shader_pass = true;

	if (gc->shader_language() == shader_glsl) // Compute shaders introduced in OpenGL 4.3
	{
		use_compute_shader_pass = gc->has_compute_shader_support();
	}
	else if (gc->shader_language() == shader_hlsl) // We need compute shaders of at least Direct3D feature level 11
	{
		use_compute_shader_pass = gc->major_version() > 10;
	}

	// use_compute_shader_pass = false; // Disable because it crashes with Oculus Rift

	passes.push_back(std::make_shared<GBufferPass>(*this));
	passes.push_back(std::make_shared<DecalsPass>(*this));
	passes.push_back(std::make_shared<SkyboxPass>(*this));
	passes.push_back(std::make_shared<ShadowMapPass>(*this));
	//passes.push_back(std::make_shared<SSAOPass>(*this));

	if (use_compute_shader_pass)
	{
		passes.push_back(std::make_shared<LightsourcePass>(*this));
	}
	else
	{
		passes.push_back(std::make_shared<LightsourceSimplePass>(*this));
	}
	
	passes.push_back(std::make_shared<TransparencyPass>(*this));
	passes.push_back(std::make_shared<ParticleEmitterPass>(*this));
	//passes.push_back(std::make_shared<LensFlarePass>(*this));
	passes.push_back(std::make_shared<CustomPass>(*this));
	passes.push_back(std::make_shared<BloomPass>(*this));
	passes.push_back(std::make_shared<LensDistortionPass>(*this));
	passes.push_back(std::make_shared<SceneLinesPass>(*this));
	passes.push_back(std::make_shared<FinalPass>(*this));
}

void CustomPass::run()
{
	if (inout.custom_pass)
	{
		Size viewport_size = inout.engine->render.viewport_size;
		Mat4f eye_to_projection = Mat4f::perspective(inout.engine->render.field_of_view, viewport_size.width / (float)viewport_size.height, 0.1f, 1.e10f, handed_left, inout.gc->clip_z_range());
		inout.custom_pass(eye_to_projection, inout.world_to_eye, inout.frames.front()->current_pipeline_fb(), inout.viewport_size);
	}
}

/////////////////////////////////////////////////////////////////////////////

void SceneRenderFrame::setup_pass_buffers(SceneRender *render)
{
	Size viewport_size = render->viewport_size;
	auto &gc = render->gc;

	if (diffuse_color_gbuffer && diffuse_color_gbuffer->size() == viewport_size && gc->is_frame_buffer_owner(fb_gbuffer))
		return;

	fb_gbuffer = nullptr;
	fb_decals = nullptr;
	fb_self_illumination = nullptr;
	for (auto &fb_blur : fb_bloom_blurv) fb_blur = nullptr;
	for (auto &fb_blur : fb_bloom_blurh) fb_blur = nullptr;
	fb_ambient_occlusion = nullptr;
	for (auto &pipeline : fb_pipeline) pipeline = nullptr;
	zbuffer = nullptr;
	diffuse_color_gbuffer = nullptr;
	specular_color_gbuffer = nullptr;
	specular_level_gbuffer = nullptr;
	self_illumination_gbuffer = nullptr;
	normal_gbuffer = nullptr;
	face_normal_z_gbuffer = nullptr;
	for (auto &blur : bloom_blurv) blur = nullptr;
	for (auto &blur : bloom_blurh) blur = nullptr;
	ambient_occlusion = nullptr;
	for (auto &pipeline : pipeline) pipeline = nullptr;

	gc->flush();

	diffuse_color_gbuffer = Texture2D::create(gc, viewport_size.width, viewport_size.height, tf_rgba8);
	specular_color_gbuffer = Texture2D::create(gc, viewport_size.width, viewport_size.height, tf_rgba8);
	specular_level_gbuffer = Texture2D::create(gc, viewport_size.width, viewport_size.height, tf_rg16f);
	self_illumination_gbuffer = Texture2D::create(gc, viewport_size.width, viewport_size.height, tf_rgba16);
	normal_gbuffer = Texture2D::create(gc, viewport_size.width, viewport_size.height, tf_rgba16f);
	face_normal_z_gbuffer = Texture2D::create(gc, viewport_size.width, viewport_size.height, tf_rgba16f);
	zbuffer = Texture2D::create(gc, viewport_size.width, viewport_size.height, tf_depth_component24);

	for (int i = 0; i < num_pipeline_buffers; i++)
		pipeline[i] = Texture2D::create(gc, viewport_size.width, viewport_size.height, tf_rgba16f);

	auto bloom_size = Size(std::max(viewport_size.width / 2, 1), std::max(viewport_size.height / 2, 1));
	for (int i = 0; i < bloom_levels; i++)
	{
		bloom_size = Size(std::max(bloom_size.width / 2, 1), std::max(bloom_size.height / 2, 1));
		bloom_blurv[i] = Texture2D::create(gc, bloom_size.width, bloom_size.height, tf_rgba16);
		bloom_blurh[i] = Texture2D::create(gc, bloom_size.width, bloom_size.height, tf_rgba16);
	}

	ambient_occlusion = Texture2D::create(gc, viewport_size.width, viewport_size.height, tf_r8);
	ambient_occlusion->set_min_filter(filter_linear);
	ambient_occlusion->set_mag_filter(filter_linear);

	fb_gbuffer = FrameBuffer::create(gc);
	fb_gbuffer->attach_color(0, diffuse_color_gbuffer);
	fb_gbuffer->attach_color(1, specular_color_gbuffer);
	fb_gbuffer->attach_color(2, specular_level_gbuffer);
	fb_gbuffer->attach_color(3, self_illumination_gbuffer);
	fb_gbuffer->attach_color(4, normal_gbuffer);
	fb_gbuffer->attach_color(5, face_normal_z_gbuffer);
	fb_gbuffer->attach_depth(zbuffer);

	fb_decals = FrameBuffer::create(gc);
	fb_decals->attach_color(0, diffuse_color_gbuffer);
	fb_decals->attach_color(1, specular_color_gbuffer);
	fb_decals->attach_color(2, specular_level_gbuffer);
	fb_decals->attach_color(3, self_illumination_gbuffer);

	fb_self_illumination = FrameBuffer::create(gc);
	fb_self_illumination->attach_color(0, self_illumination_gbuffer);
	fb_self_illumination->attach_depth(zbuffer);

	for (int i = 0; i < num_pipeline_buffers; i++)
	{
		fb_pipeline[i] = FrameBuffer::create(gc);
		fb_pipeline[i]->attach_color(0, pipeline[i]);
		fb_pipeline[i]->attach_depth(zbuffer);
	}

	for (int i = 0; i < bloom_levels; i++)
	{
		fb_bloom_blurv[i] = FrameBuffer::create(gc);
		fb_bloom_blurv[i]->attach_color(0, bloom_blurv[i]);

		fb_bloom_blurh[i] = FrameBuffer::create(gc);
		fb_bloom_blurh[i]->attach_color(0, bloom_blurh[i]);
	}

	fb_ambient_occlusion = FrameBuffer::create(gc);
	fb_ambient_occlusion->attach_color(0, ambient_occlusion);
}
