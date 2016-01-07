
#include "precomp.h"
#include "scene_render.h"
#include "Scene3D/Performance/scope_timer.h"
#include "Scene3D/Scene/scene_camera_impl.h"
#include "Scene3D/SceneEngine/scene_viewport_impl.h"

using namespace uicore;

SceneRender::SceneRender(SceneEngineImpl *engine) : engine(engine), shadow_maps(*this)
{
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

	viewport = scene_viewport->_viewport;
	fb_viewport = scene_viewport->_fb_viewport;

	field_of_view = camera->field_of_view();
	Quaternionf inv_orientation = Quaternionf::inverse(scene_viewport->camera()->orientation());
	world_to_eye = inv_orientation.to_matrix() * Mat4f::translate(-scene_viewport->camera()->position());

	gc = render_gc;
	gpu_timer.begin_frame(gc);

	setup_passes();
	setup_pass_buffers();

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

void SceneRender::setup_passes()
{
	if (!passes.empty())
		return;

	bool use_compute_shader_pass = false;

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
	passes.push_back(std::make_shared<SkyboxPass>(*this));
	passes.push_back(std::make_shared<VSMShadowMapPass>(gc, *this));

	if (use_compute_shader_pass)
	{
		passes.push_back(std::make_shared<LightsourcePass>(gc, *this));
	}
	else
	{
		passes.push_back(std::make_shared<LightsourceSimplePass>(gc, *this));
	}

	passes.push_back(std::make_shared<TransparencyPass>(*this));
	passes.push_back(std::make_shared<ParticleEmitterPass>(*this));
	//passes.push_back(std::make_shared<LensFlarePass>(*this));
	passes.push_back(std::make_shared<BloomPass>(gc, *this));
	//passes.push_back(std::make_shared<SSAOPass>(gc, *this));
	passes.push_back(std::make_shared<SceneLinesPass>(*this));
	passes.push_back(std::make_shared<FinalPass>(gc, *this));
}

void SceneRender::setup_pass_buffers()
{
	Size viewport_size = viewport.size();

	if (diffuse_color_gbuffer && diffuse_color_gbuffer->size() == viewport_size && gc->is_frame_buffer_owner(fb_gbuffer))
		return;

	fb_gbuffer = nullptr;
	fb_self_illumination = nullptr;
	for (auto &fb_blur : fb_bloom_blurv) fb_blur = nullptr;
	for (auto &fb_blur : fb_bloom_blurh) fb_blur = nullptr;
	fb_ambient_occlusion = nullptr;
	fb_final_color = nullptr;
	zbuffer = nullptr;
	diffuse_color_gbuffer = nullptr;
	specular_color_gbuffer = nullptr;
	specular_level_gbuffer = nullptr;
	self_illumination_gbuffer = nullptr;
	normal_z_gbuffer = nullptr;
	for (auto &blur : bloom_blurv) blur = nullptr;
	for (auto &blur : bloom_blurh) blur = nullptr;
	ambient_occlusion = nullptr;
	final_color = nullptr;

	gc->flush();

	diffuse_color_gbuffer = Texture2D::create(gc, viewport_size.width, viewport_size.height, tf_rgba8);
	specular_color_gbuffer = Texture2D::create(gc, viewport_size.width, viewport_size.height, tf_rgba8);
	specular_level_gbuffer = Texture2D::create(gc, viewport_size.width, viewport_size.height, tf_rg16f);
	self_illumination_gbuffer = Texture2D::create(gc, viewport_size.width, viewport_size.height, tf_rgba16);
	normal_z_gbuffer = Texture2D::create(gc, viewport_size.width, viewport_size.height, tf_rgba16f);
	zbuffer = Texture2D::create(gc, viewport_size.width, viewport_size.height, tf_depth_component24);

	final_color = Texture2D::create(gc, viewport_size.width, viewport_size.height, tf_rgba16f);

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
	fb_gbuffer->attach_color(4, normal_z_gbuffer);
	fb_gbuffer->attach_depth(zbuffer);

	fb_self_illumination = FrameBuffer::create(gc);
	fb_self_illumination->attach_color(0, self_illumination_gbuffer);
	fb_self_illumination->attach_depth(zbuffer);

	fb_final_color = FrameBuffer::create(gc);
	fb_final_color->attach_color(0, final_color);
	fb_final_color->attach_depth(zbuffer);

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
