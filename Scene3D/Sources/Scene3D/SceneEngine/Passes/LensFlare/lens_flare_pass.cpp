
#include "precomp.h"
#include "lens_flare_pass.h"
#include "Scene3D/SceneEngine/scene_viewport_impl.h"
#include "Scene3D/Scene/scene_impl.h"
#include "Scene3D/Scene/scene_light_impl.h"
#include "vertex_lens_flare_hlsl.h"
#include "fragment_lens_flare_hlsl.h"
#include "log_event.h"
#include "Scene3D/Performance/scope_timer.h"

using namespace uicore;

LensFlarePass::LensFlarePass(SceneRender &inout) : inout(inout)
{
}

void LensFlarePass::run()
{
	ScopeTimeFunction();

	setup();

	if (!flare_texture.get())
		flare_texture = inout.engine->get_texture(inout.gc, "lensflare.png", false);

	Size viewport_size = inout.viewport.size();
	Mat4f eye_to_projection = Mat4f::perspective(inout.field_of_view, viewport_size.width / (float)viewport_size.height, 0.1f, 1.e10f, handed_left, inout.gc->clip_z_range());
	Mat4f eye_to_cull_projection = Mat4f::perspective(inout.field_of_view, viewport_size.width / (float)viewport_size.height, 0.1f, 150.0f, handed_left, clip_negative_positive_w);
	FrustumPlanes frustum(eye_to_cull_projection * inout.world_to_eye);

	std::vector<SceneLightImpl*> lights;
	inout.scene->foreach_light(frustum, [&](SceneLightImpl *light) { lights.push_back(light); });

	if (lights.empty())
		return;

	if (!instance_texture || instance_texture->width() < (int)lights.size())
	{
		instance_texture = nullptr;
		for (int i = 0; i < num_instance_transfer; i++)
			instance_transfer[i] = nullptr;

		instance_texture = Texture2D::create(inout.gc, (int)lights.size(), 1, tf_rgba32f);
		for (int i = 0; i < num_instance_transfer; i++)
			instance_transfer[i] = StagingTexture::create(inout.gc, (int)lights.size(), 1, StagingDirection::to_gpu, tf_rgba32f, 0, usage_stream_draw);
	}

	instance_transfer[current_instance_transfer]->lock(inout.gc, access_write_discard);
	auto instance_data = instance_transfer[current_instance_transfer]->data<Vec4f>();
	for (size_t i = 0; i < lights.size(); i++)
		instance_data[i] = Vec4f(lights[i]->position(), lights[i]->attenuation_end()/7.0f);
	instance_transfer[current_instance_transfer]->unlock();
	instance_texture->set_image(inout.gc, instance_transfer[current_instance_transfer]);
	current_instance_transfer = (current_instance_transfer + 1) % num_instance_transfer;

	UniformBlock uniforms;
	uniforms.eye_to_projection = eye_to_projection;
	uniforms.object_to_eye = inout.world_to_eye;
	gpu_uniforms.upload_data(inout.gc, &uniforms, 1);

	inout.gc->set_frame_buffer(inout.frames.front()->fb_final_color);
	inout.gc->set_viewport(viewport_size, inout.gc->texture_image_y_axis());

	inout.gc->set_depth_range(0.0f, 0.9f);
	inout.gc->set_depth_stencil_state(depth_stencil_state);
	inout.gc->set_blend_state(blend_state);

	inout.gc->set_program_object(program);

	inout.gc->set_uniform_buffer(0, gpu_uniforms);
	inout.gc->set_texture(0, instance_texture);
	inout.gc->set_texture(1, flare_texture.get());

	inout.gc->set_primitives_array(prim_array);
	inout.gc->draw_primitives_array_instanced(type_triangles, 0, 6, (int)lights.size());
	inout.gc->reset_primitives_array();

	inout.gc->reset_texture(0);
	inout.gc->reset_texture(1);
	inout.gc->reset_texture(2);
	inout.gc->reset_uniform_buffer(0);

	inout.gc->reset_program_object();
	inout.gc->reset_rasterizer_state();
	inout.gc->reset_depth_stencil_state();
	inout.gc->set_depth_range(0.0f, 1.0f);

	inout.gc->reset_frame_buffer();
}

void LensFlarePass::setup()
{
	if (!program)
	{
		auto vertex_shader = ShaderObject::create(inout.gc, ShaderType::vertex, vertex_lens_flare_hlsl());
		vertex_shader->compile();

		auto fragment_shader = ShaderObject::create(inout.gc, ShaderType::fragment, fragment_lens_flare_hlsl());
		fragment_shader->compile();

		program = ProgramObject::create(inout.gc);
		program->attach(vertex_shader);
		program->attach(fragment_shader);
		program->bind_attribute_location(0, "AttrPosition");
		program->bind_frag_data_location(0, "FragColor");
		if (!program->try_link())
			throw Exception(string_format("Lens flare program failed to link: %1", program->info_log()));
		program->set_uniform_buffer_index("Uniforms", 0);
		program->set_uniform1i("InstanceTexture", 0);
		program->set_uniform1i("FlareTexture", 1);
		program->set_uniform1i("FlareSampler", 1);

		std::vector<Vec3f> cpu_billboard_positions =
		{
			Vec3f(-1.0f, -1.0f, 0.0f),
			Vec3f(1.0f, -1.0f, 0.0f),
			Vec3f(-1.0f, 1.0f, 0.0f),
			Vec3f(-1.0f, 1.0f, 0.0f),
			Vec3f(1.0f, -1.0f, 0.0f),
			Vec3f(1.0f, 1.0f, 0.0f)
		};

		billboard_positions = VertexArrayVector<Vec3f>(inout.gc, cpu_billboard_positions);
		gpu_uniforms = uicore::UniformVector<UniformBlock>(inout.gc, 1);

		BlendStateDescription blend_desc;
		blend_desc.enable_blending(true);
		blend_desc.set_blend_function(blend_one, blend_one, blend_zero, blend_one);
		blend_state = inout.gc->create_blend_state(blend_desc);

		DepthStencilStateDescription depth_stencil_desc;
		depth_stencil_desc.enable_depth_write(false);
		depth_stencil_desc.enable_depth_test(true); // To do: this should be false and then we should use occlusion queries to fade flares in and out
		depth_stencil_state = inout.gc->create_depth_stencil_state(depth_stencil_desc);

		RasterizerStateDescription rasterizer_desc;
		rasterizer_desc.set_culled(false);
		rasterizer_state = inout.gc->create_rasterizer_state(rasterizer_desc);

		prim_array = PrimitivesArray::create(inout.gc);
		prim_array->set_attributes(0, billboard_positions);
	}
}
