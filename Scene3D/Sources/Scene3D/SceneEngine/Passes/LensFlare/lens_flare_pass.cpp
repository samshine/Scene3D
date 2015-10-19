
#include "precomp.h"
#include "lens_flare_pass.h"
#include "Scene3D/Scene/scene_impl.h"
#include "Scene3D/Scene/scene_light_impl.h"

using namespace uicore;

LensFlarePass::LensFlarePass(const std::string &shader_path, SceneRender &inout) : shader_path(shader_path), inout(inout)
{
}

void LensFlarePass::run(const GraphicContextPtr &gc, SceneImpl *scene)
{
	setup(gc);

	if (!flare_texture.get())
		flare_texture = scene->engine()->get_texture(gc, "lensflare.png", false);

	Size viewport_size = inout.viewport.size();
	Mat4f eye_to_projection = Mat4f::perspective(inout.field_of_view, viewport_size.width / (float)viewport_size.height, 0.1f, 1.e10f, handed_left, gc->clip_z_range());
	Mat4f eye_to_cull_projection = Mat4f::perspective(inout.field_of_view, viewport_size.width / (float)viewport_size.height, 0.1f, 150.0f, handed_left, clip_negative_positive_w);
	FrustumPlanes frustum(eye_to_cull_projection * inout.world_to_eye);

	std::vector<SceneLightImpl*> lights;
	scene->foreach_light(frustum, [&](SceneLightImpl *light) { lights.push_back(light); });

	if (!instance_texture || instance_texture->width() < (int)lights.size())
	{
		instance_texture = Texture2D::create(gc, (int)lights.size(), 1, tf_rgba32f);
		instance_transfer = StagingTexture::create(gc, (int)lights.size(), 1, StagingDirection::to_gpu, tf_rgba32f, 0, usage_stream_draw);
	}

	instance_transfer->lock(gc, access_write_discard);
	auto instance_data = instance_transfer->data<Vec4f>();
	for (size_t i = 0; i < lights.size(); i++)
		instance_data[i] = Vec4f(lights[i]->position(), lights[i]->attenuation_end()/7.0f);
	instance_transfer->unlock();
	instance_texture->set_image(gc, instance_transfer);

	UniformBlock uniforms;
	uniforms.eye_to_projection = eye_to_projection;
	uniforms.object_to_eye = inout.world_to_eye;
	gpu_uniforms.upload_data(gc, &uniforms, 1);

	gc->set_frame_buffer(inout.fb_final_color);
	gc->set_viewport(viewport_size, gc->texture_image_y_axis());

	gc->set_depth_range(0.0f, 0.9f);
	gc->set_depth_stencil_state(depth_stencil_state);
	gc->set_blend_state(blend_state);

	gc->set_program_object(program);

	gc->set_uniform_buffer(0, gpu_uniforms);
	gc->set_texture(0, instance_texture);
	gc->set_texture(1, flare_texture.get());

	gc->set_primitives_array(prim_array);
	gc->draw_primitives_array_instanced(type_triangles, 0, 6, (int)lights.size());
	gc->reset_primitives_array();

	gc->reset_texture(0);
	gc->reset_texture(1);
	gc->reset_texture(2);
	gc->reset_uniform_buffer(0);

	gc->reset_program_object();
	gc->reset_rasterizer_state();
	gc->reset_depth_stencil_state();
	gc->set_depth_range(0.0f, 1.0f);

	gc->reset_frame_buffer();
}

void LensFlarePass::setup(const GraphicContextPtr &gc)
{
	if (!program)
	{
		std::string vertex_filename = PathHelp::combine(shader_path, "LensFlare/vertex.hlsl");
		std::string fragment_filename = PathHelp::combine(shader_path, "LensFlare/fragment.hlsl");
		program = ProgramObject::load(gc, vertex_filename, fragment_filename);
		program->bind_attribute_location(0, "AttrPosition");
		program->bind_frag_data_location(0, "FragColor");
		if (!program->try_link())
			throw Exception(string_format("Lens flare program failed to link: %1", program->get_info_log()));
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

		billboard_positions = VertexArrayVector<Vec3f>(gc, cpu_billboard_positions);
		gpu_uniforms = uicore::UniformVector<UniformBlock>(gc, 1);

		BlendStateDescription blend_desc;
		blend_desc.enable_blending(true);
		blend_desc.set_blend_function(blend_one, blend_one, blend_zero, blend_one);
		blend_state = gc->create_blend_state(blend_desc);

		DepthStencilStateDescription depth_stencil_desc;
		depth_stencil_desc.enable_depth_write(false);
		depth_stencil_desc.enable_depth_test(true); // To do: this should be false and then we should use occlusion queries to fade flares in and out
		depth_stencil_state = gc->create_depth_stencil_state(depth_stencil_desc);

		RasterizerStateDescription rasterizer_desc;
		rasterizer_desc.set_culled(false);
		rasterizer_state = gc->create_rasterizer_state(rasterizer_desc);

		prim_array = PrimitivesArray::create(gc);
		prim_array->set_attributes(0, billboard_positions);
	}
}
