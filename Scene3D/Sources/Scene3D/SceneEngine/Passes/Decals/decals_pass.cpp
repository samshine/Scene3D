
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

	Size viewport_size = inout.viewport.size();
	Mat4f eye_to_cull_projection = Mat4f::perspective(inout.field_of_view, viewport_size.width / (float)viewport_size.height, 0.1f, 150.0f, handed_left, clip_negative_positive_w);
	FrustumPlanes frustum(eye_to_cull_projection * inout.world_to_eye);

	inout.scene->foreach_decal(frustum, [&](SceneDecalImpl *decal) { render_lists[decal->diffuse_texture()].push_back(decal); });

	if (render_lists.empty())
		return;

	inout.gc->set_frame_buffer(inout.frames.front()->fb_decals);
	inout.gc->set_viewport(viewport_size, inout.gc->texture_image_y_axis());

	inout.gc->set_depth_range(0.0f, 0.9f);
	inout.gc->set_depth_stencil_state(depth_stencil_state);
	inout.gc->set_blend_state(blend_state);
	inout.gc->set_rasterizer_state(rasterizer_state);
	inout.gc->set_program_object(program);

	render_decals();

	inout.gc->reset_program_object();
	inout.gc->reset_rasterizer_state();
	inout.gc->reset_blend_state();
	inout.gc->reset_depth_stencil_state();
	inout.gc->set_depth_range(0.0f, 1.0f);

	inout.gc->reset_frame_buffer();
}

void DecalsPass::render_decals()
{
	int vectors_pos = 0;
	std::vector<InstanceBatch> batches;

	for (auto &it : render_lists)
	{
		auto diffuse_texture = it.first;
		auto &instances = it.second;

		size_t pos = 0;
		while (pos < instances.size())
		{
			int left = (int)(instances.size() - pos);
			int count = std::min((num_vectors - vectors_pos) / vectors_per_instance, left);
			if (count == 0)
			{
				render_batches(batches);
				batches.clear();
				vectors_pos = 0;
			}
			else
			{
				InstanceBatch batch;
				batch.diffuse_texture = diffuse_texture;
				batch.start = vectors_pos;
				batch.count = count;
				batch.decals = instances.data() + pos;
				batches.push_back(batch);

				pos += count;
				vectors_pos += count * vectors_per_instance;
			}
		}
	}

	if (!batches.empty())
		render_batches(batches);

	render_lists.clear();
}

void DecalsPass::render_batches(const std::vector<InstanceBatch> &batches)
{
	StagingTexturePtr staging_buffer = get_staging_buffer(num_vectors);

	staging_buffer->lock(inout.gc, access_write_discard);
	Vec4f *vectors = staging_buffer->data<Vec4f>();

	for (const auto &batch : batches)
	{
		int write_pos = batch.start;

		for (int i = 0; i < batch.count; i++)
		{
			write_instance_data(batch.diffuse_texture, batch.decals[i], vectors + write_pos);
			write_pos += vectors_per_instance;
		}
	}

	staging_buffer->unlock();

	Texture2DPtr instance_buffer = get_instance_buffer(num_vectors);
	instance_buffer->set_subimage(inout.gc, 0, 0, staging_buffer, staging_buffer->size());

	for (const auto &batch : batches)
	{
		render_cube(batch.diffuse_texture, instance_buffer, batch.start, batch.count);
	}
}

void DecalsPass::write_instance_data(const std::string &diffuse_texture, SceneDecalImpl *decal, uicore::Vec4f *instance_data)
{
	Mat4f object_to_eye = inout.world_to_eye * uicore::Mat4f::translate(decal->position()) * decal->orientation().to_matrix() * uicore::Mat4f::scale(decal->extents());

	instance_data[0] = Vec4f(object_to_eye[0], object_to_eye[4], object_to_eye[8], object_to_eye[12]);
	instance_data[1] = Vec4f(object_to_eye[1], object_to_eye[5], object_to_eye[9], object_to_eye[13]);
	instance_data[2] = Vec4f(object_to_eye[2], object_to_eye[6], object_to_eye[10], object_to_eye[14]);
	instance_data[3] = Vec4f(std::cos(decal->cutoff_angle()), decal->glossiness(), decal->specular_level(), 0.0f);
}

void DecalsPass::render_cube(const std::string &diffuse_texture, uicore::Texture2DPtr instance_buffer, int instance_base, int instance_count)
{
	auto &texture = diffuse_textures[diffuse_texture];
	if (!texture.get())
		texture = inout.engine->get_texture(inout.gc, diffuse_texture, false);

	Size viewport_size = inout.viewport.size();
	Mat4f eye_to_projection = Mat4f::perspective(inout.field_of_view, viewport_size.width / (float)viewport_size.height, 0.1f, 1.e10f, handed_left, inout.gc->clip_z_range());
	Mat4f eye_to_cull_projection = Mat4f::perspective(inout.field_of_view, viewport_size.width / (float)viewport_size.height, 0.1f, 150.0f, handed_left, clip_negative_positive_w);
	FrustumPlanes frustum(eye_to_cull_projection * inout.world_to_eye);

	float aspect = inout.viewport.width() / (float)inout.viewport.height();
	float field_of_view_y_degrees = inout.field_of_view;
	float field_of_view_y_rad = (float)(field_of_view_y_degrees * PI / 180.0);
	float f = 1.0f / tan(field_of_view_y_rad * 0.5f);
	float rcp_f = 1.0f / f;
	float rcp_f_div_aspect = 1.0f / (f / aspect);
	Vec2f two_rcp_viewport_size(2.0f / inout.viewport.width(), 2.0f / inout.viewport.height());

	auto gpu_uniforms = get_uniform_buffer();

	DecalRenderUniforms uniforms;
	uniforms.eye_to_projection = eye_to_projection;
	uniforms.rcp_f = rcp_f;
	uniforms.rcp_f_div_aspect = rcp_f_div_aspect;
	uniforms.two_rcp_viewport_size = two_rcp_viewport_size;
	gpu_uniforms.upload_data(inout.gc, &uniforms, 1);

	inout.gc->set_uniform_buffer(0, gpu_uniforms);
	inout.gc->set_texture(0, instance_buffer);
	inout.gc->set_texture(1, texture);
	inout.gc->set_texture(2, inout.frames.front()->face_normal_z_gbuffer);

	inout.gc->set_primitives_array(prim_array);
	inout.gc->draw_primitives_array_instanced(type_triangles, 0, 6 * 6, instance_count);
	inout.gc->reset_primitives_array();

	inout.gc->reset_texture(0);
	inout.gc->reset_texture(1);
	inout.gc->reset_texture(2);
	inout.gc->reset_uniform_buffer(0);
}

Texture2DPtr DecalsPass::get_instance_buffer(int size)
{
	auto &instance_buffers = inout.frames.front()->decal_instance_buffers;
	auto &next = inout.frames.front()->next_decal_instance_buffer;

	if (instance_buffers.size() <= next)
		instance_buffers.resize(next + 1);
	auto &buffer = instance_buffers[next];

	auto height_needed = std::max((size + 255) / 256, 1);
	if (!buffer || buffer->size().height < height_needed)
		buffer = Texture2D::create(inout.gc, 256, height_needed, tf_rgba32f);

	next++;
	return buffer;
}

StagingTexturePtr DecalsPass::get_staging_buffer(int size)
{
	auto &staging_buffers = inout.frames.front()->decal_staging_buffers;
	auto &next = inout.frames.front()->next_decal_staging_buffer;

	if (staging_buffers.size() <= next)
		staging_buffers.resize(next + 1);
	auto &buffer = staging_buffers[next];

	auto height_needed = std::max((size + 255) / 256, 1);
	if (!buffer || buffer->size().height < height_needed)
		buffer = StagingTexture::create(inout.gc, 256, height_needed, StagingDirection::to_gpu, tf_rgba32f);

	next++;
	return buffer;
}

UniformVector<DecalRenderUniforms> DecalsPass::get_uniform_buffer()
{
	auto &render_uniforms = inout.frames.front()->decal_render_uniforms;
	auto &next = inout.frames.front()->next_decal_render_uniforms;

	if (render_uniforms.size() <= next)
		render_uniforms.resize(next + 1);

	auto &buffer = render_uniforms[next];
	if (!buffer.buffer())
		buffer = UniformVector<DecalRenderUniforms>(inout.gc, 1);
	next++;
	return buffer;
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
			throw Exception(string_format("Decals program failed to link: %1", program->info_log()));
		program->set_uniform_buffer_index("Uniforms", 0);
		program->set_uniform1i("InstanceTexture", 0);
		program->set_uniform1i("DiffuseTexture", 1);
		program->set_uniform1i("DiffuseSampler", 1);
		program->set_uniform1i("FaceNormalZTexture", 2);

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
