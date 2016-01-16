
#include "precomp.h"
#include "decals_pass.h"
#include "Scene3D/scene.h"
#include "Scene3D/Scene/scene_impl.h"
#include "Scene3D/Scene/scene_decal_impl.h"
#include "Scene3D/SceneEngine/scene_viewport_impl.h"
#include "Scene3D/Performance/scope_timer.h"
#include "vertex_decals_glsl.h"
#include "vertex_decals_hlsl.h"
#include "fragment_decals_glsl.h"
#include "fragment_decals_hlsl.h"

using namespace uicore;

DecalsPass::DecalsPass(SceneRender &inout) : inout(inout)
{
}

void DecalsPass::run()
{
	ScopeTimeFunction();

	// To do: implement http://www.slideshare.net/blindrenderer/screen-space-decals-in-warhammer-40000-space-marine-14699854

	setup();

	if (!diffuse_texture.get())
		diffuse_texture = inout.engine->get_texture(inout.gc, "lensflare.png", false);

	Size viewport_size = inout.viewport.size();
	Mat4f eye_to_projection = Mat4f::perspective(inout.field_of_view, viewport_size.width / (float)viewport_size.height, 0.1f, 1.e10f, handed_left, inout.gc->clip_z_range());
	Mat4f eye_to_cull_projection = Mat4f::perspective(inout.field_of_view, viewport_size.width / (float)viewport_size.height, 0.1f, 150.0f, handed_left, clip_negative_positive_w);
	FrustumPlanes frustum(eye_to_cull_projection * inout.world_to_eye);

	std::vector<SceneDecalImpl*> decals;
	inout.scene->foreach_decal(frustum, [&](SceneDecalImpl *decal) { decals.push_back(decal); });

	if (decals.empty())
		return;

	if (!instance_texture || instance_texture->width() * instance_texture->height() < (int)decals.size())
	{
		instance_texture = nullptr;
		for (int i = 0; i < num_instance_transfer; i++)
			instance_transfer[i] = nullptr;

		int vectors = (int)decals.size() * vectors_per_instance;
		int height = std::max((vectors + 255) / 256, 1);

		instance_texture = Texture2D::create(inout.gc, 256, height, tf_rgba32f);
		for (int i = 0; i < num_instance_transfer; i++)
			instance_transfer[i] = StagingTexture::create(inout.gc, 256, height, StagingDirection::to_gpu, tf_rgba32f, 0, usage_stream_draw);
	}

	instance_transfer[current_instance_transfer]->lock(inout.gc, access_write_discard);
	auto instance_data = instance_transfer[current_instance_transfer]->data<Vec4f>();
	for (const auto &decal : decals)
	{
		Mat4f object_to_eye = inout.world_to_eye * uicore::Mat4f::translate(decal->position()) * decal->orientation().to_matrix() * uicore::Mat4f::scale(decal->extents());

		instance_data[0] = Vec4f(object_to_eye[0], object_to_eye[4], object_to_eye[8], object_to_eye[12]);
		instance_data[1] = Vec4f(object_to_eye[1], object_to_eye[5], object_to_eye[9], object_to_eye[13]);
		instance_data[2] = Vec4f(object_to_eye[2], object_to_eye[6], object_to_eye[10], object_to_eye[14]);
		instance_data[3] = Vec4f(decal->cutoff_angle(), decal->glossiness(), decal->specular_level(), 0.0f);

		instance_data += vectors_per_instance;
	}
	instance_transfer[current_instance_transfer]->unlock();
	instance_texture->set_image(inout.gc, instance_transfer[current_instance_transfer]);
	current_instance_transfer = (current_instance_transfer + 1) % num_instance_transfer;

	UniformBlock uniforms;
	uniforms.eye_to_projection = eye_to_projection;
	gpu_uniforms.upload_data(inout.gc, &uniforms, 1);

	inout.gc->set_frame_buffer(inout.fb_gbuffer);
	inout.gc->set_viewport(viewport_size, inout.gc->texture_image_y_axis());

	inout.gc->set_depth_range(0.0f, 0.9f);
	inout.gc->set_depth_stencil_state(depth_stencil_state);
	inout.gc->set_blend_state(blend_state);

	inout.gc->set_program_object(program);

	inout.gc->set_uniform_buffer(0, gpu_uniforms);
	inout.gc->set_texture(0, instance_texture);
	inout.gc->set_texture(1, diffuse_texture.get());

	inout.gc->set_primitives_array(prim_array);
	inout.gc->draw_primitives_array_instanced(type_triangles, 0, 6 * 6, (int)decals.size());
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

void DecalsPass::setup()
{
	if (!program)
	{
		auto vertex_shader = ShaderObject::create(inout.gc, ShaderType::vertex, vertex_decals_hlsl());
		vertex_shader->compile();

		auto fragment_shader = ShaderObject::create(inout.gc, ShaderType::fragment, fragment_decals_hlsl());
		fragment_shader->compile();

		program = ProgramObject::create(inout.gc);
		program->attach(vertex_shader);
		program->attach(fragment_shader);
		program->bind_attribute_location(0, "AttrPosition");
		program->bind_frag_data_location(0, "FragColor");
		if (!program->try_link())
			throw Exception(string_format("Decals program failed to link: %1", program->get_info_log()));
		program->set_uniform_buffer_index("Uniforms", 0);
		program->set_uniform1i("InstanceTexture", 0);
		program->set_uniform1i("DiffuseTexture", 1);
		program->set_uniform1i("DiffuseSampler", 1);

		std::vector<Vec3f> cpu_box_positions =
		{
			Vec3f(-1.0f, 1.0f, 1.0f),
			Vec3f(1.0f, 1.0f, 1.0f),
			Vec3f(1.0f, -1.0f, 1.0f),

			Vec3f(1.0f, -1.0f, 1.0f),
			Vec3f(-1.0f, -1.0f, 1.0f),
			Vec3f(-1.0f, 1.0f, 1.0f),

			Vec3f(1.0f, -1.0f, -1.0f),
			Vec3f(1.0f, 1.0f, -1.0f),
			Vec3f(-1.0f, 1.0f, -1.0f),

			Vec3f(-1.0f, 1.0f, -1.0f),
			Vec3f(-1.0f, -1.0f, -1.0f),
			Vec3f(1.0f, -1.0f, -1.0f),

			Vec3f(1.0f, 1.0f, -1.0f),
			Vec3f(1.0f, 1.0f, 1.0f),
			Vec3f(-1.0f, 1.0f, 1.0f),

			Vec3f(-1.0f, 1.0f, 1.0f),
			Vec3f(-1.0f, 1.0f, -1.0f),
			Vec3f(1.0f, 1.0f, -1.0f),

			Vec3f(-1.0f, -1.0f, 1.0f),
			Vec3f(1.0f, -1.0f, 1.0f),
			Vec3f(1.0f, -1.0f, -1.0f),

			Vec3f(1.0f, -1.0f, -1.0f),
			Vec3f(-1.0f, -1.0f, -1.0f),
			Vec3f(-1.0f, -1.0f, 1.0f),

			Vec3f(1.0f, -1.0f, 1.0f),
			Vec3f(1.0f, 1.0f, 1.0f),
			Vec3f(1.0f, 1.0f, -1.0f),

			Vec3f(1.0f, 1.0f, -1.0f),
			Vec3f(1.0f, -1.0f, -1.0f),
			Vec3f(1.0f, -1.0f, 1.0f),

			Vec3f(-1.0f, 1.0f, -1.0f),
			Vec3f(-1.0f, 1.0f, 1.0f),
			Vec3f(-1.0f, -1.0f, 1.0f),

			Vec3f(-1.0f, -1.0f, 1.0f),
			Vec3f(-1.0f, -1.0f, -1.0f),
			Vec3f(-1.0f, 1.0f, -1.0f)
		};

		box_positions = VertexArrayVector<Vec3f>(inout.gc, cpu_box_positions);
		gpu_uniforms = uicore::UniformVector<UniformBlock>(inout.gc, 1);

		BlendStateDescription blend_desc;
		blend_desc.enable_blending(true);
		blend_desc.set_blend_function(blend_one, blend_one_minus_src_alpha, blend_zero, blend_one);
		blend_state = inout.gc->create_blend_state(blend_desc);

		DepthStencilStateDescription depth_stencil_desc;
		depth_stencil_desc.enable_depth_write(false);
		depth_stencil_desc.enable_depth_test(true);
		depth_stencil_state = inout.gc->create_depth_stencil_state(depth_stencil_desc);

		RasterizerStateDescription rasterizer_desc;
		rasterizer_desc.set_culled(true);
		rasterizer_state = inout.gc->create_rasterizer_state(rasterizer_desc);

		prim_array = PrimitivesArray::create(inout.gc);
		prim_array->set_attributes(0, box_positions);
	}
}
