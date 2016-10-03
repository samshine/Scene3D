
#include "precomp.h"
#include "particle_emitter_pass.h"
#include "Scene3D/scene.h"
#include "Scene3D/SceneEngine/mapped_buffer.h"
#include "Scene3D/Performance/scope_timer.h"
#include "Scene3D/Scene/scene_camera_impl.h"
#include "Scene3D/Scene/scene_impl.h"
#include <algorithm>
#include "vertex_particle_emitter_hlsl.h"
#include "vertex_particle_emitter_glsl.h"
#include "fragment_particle_emitter_hlsl.h"
#include "fragment_particle_emitter_glsl.h"
#include "log_event.h"

using namespace uicore;

ParticleEmitterPass::ParticleEmitterPass(SceneRender &render) : inout(render)
{
	emitter_slots.resize(32);
	for (size_t i = 0; i < emitter_slots.size(); i++)
		free_emitters.push_back((int)i);
}

void ParticleEmitterPass::select_active_emitters(const FrustumPlanes &frustum)
{
	std::vector<SceneParticleEmitterImpl*> visible_emitters;
	inout.scene->foreach_emitter(frustum, [&](SceneParticleEmitterImpl *emitter) { visible_emitters.push_back(emitter); });

	auto camera_pos = inout.camera->position();
	std::sort(visible_emitters.begin(), visible_emitters.end(), [&](SceneParticleEmitterImpl *a, SceneParticleEmitterImpl *b)
	{
		Vec3f vec_a = a->position() - camera_pos;
		Vec3f vec_b = b->position() - camera_pos;
		return Vec3f::dot(vec_a, vec_a) < Vec3f::dot(vec_b, vec_b);
	});

	for (auto slot : active_emitters)
		emitter_slots[slot].visible = false;

	size_t max_emitters = active_emitters.size() + free_emitters.size();
	size_t count = std::min(max_emitters, visible_emitters.size());
	for (size_t i = 0; i < count; i++)
	{
		auto emitter = visible_emitters[i];
		if (emitter->pass_data)
		{
			emitter->pass_data->visible = true;
		}
		else if (!free_emitters.empty())
		{
			int slot = free_emitters.back();
			active_emitters.push_back(slot);
			free_emitters.pop_back();

			emitter->pass_data = &emitter_slots[slot];
			emitter->pass_data->emitter = emitter;
			emitter->pass_data->visible = true;
			emitter->pass_data->life_color_gradient = inout.engine->get_texture(inout.gc, emitter->gradient_texture(), false);
			emitter->pass_data->particle_animation = inout.engine->get_texture(inout.gc, emitter->particle_texture(), false);
			if (!emitter->pass_data->gpu_uniforms.buffer())
				emitter->pass_data->gpu_uniforms = UniformVector<ParticleUniforms>(inout.gc, 1);
		}
	}
}

void ParticleEmitterPass::run()
{
	ScopeTimeFunction();

	setup();

	Size viewport_size = inout.engine->render.viewport_size;
	Mat4f eye_to_projection = Mat4f::perspective(inout.engine->render.field_of_view, viewport_size.width / (float)viewport_size.height, 0.1f, 1.e10f, handed_left, inout.gc->clip_z_range());
	Mat4f eye_to_cull_projection = Mat4f::perspective(inout.engine->render.field_of_view, viewport_size.width / (float)viewport_size.height, 0.1f, 150.0f, handed_left, clip_negative_positive_w);
	FrustumPlanes frustum(eye_to_cull_projection * inout.engine->render.world_to_eye);

	select_active_emitters(frustum);

	const int vectors_per_particle = 2;

	size_t total_particle_count = 0;
	for (int slot : active_emitters)
	{
		auto pass_data = &emitter_slots[slot];
		if (pass_data->cpu_particles.empty())
			continue;

		float depth_fade_distance = 1.0f;
		ParticleUniforms uniforms;
		uniforms.eye_to_projection = eye_to_projection;
		uniforms.object_to_eye = inout.engine->render.world_to_eye;
		uniforms.rcp_depth_fade_distance = 1.0f / depth_fade_distance;
		uniforms.instance_vectors_offset = total_particle_count * vectors_per_particle;
		pass_data->gpu_uniforms.upload_data(inout.gc, &uniforms, 1);

		total_particle_count += pass_data->cpu_particles.size();
	}

	if (total_particle_count == 0)
		return;

	auto &instance_texture = inout.frames.front()->particle_instance_texture;
	auto &instance_transfer = inout.frames.front()->particle_instance_transfer;

	if (!instance_texture || instance_texture->width() < (int)total_particle_count * vectors_per_particle)
	{
		instance_texture = nullptr;
		instance_transfer = nullptr;

		instance_texture = Texture2D::create(inout.gc, total_particle_count * vectors_per_particle, 1, tf_rgba32f);
		instance_transfer = StagingTexture::create(inout.gc, total_particle_count * vectors_per_particle, 1, StagingDirection::to_gpu, tf_rgba32f, 0, usage_stream_draw);
	}

	instance_transfer->lock(inout.gc, access_write_discard);

	MappedBuffer<Vec4f> vectors(instance_transfer->data<Vec4f>(), instance_transfer->width() * instance_transfer->height());
	size_t vector_offset = 0;
	for (int slot : active_emitters)
	{
		auto pass_data = &emitter_slots[slot];
		if (pass_data->cpu_particles.empty())
			continue;

		Vec3f eye_pos = inout.camera->position();
		std::vector<ParticleOrderIndex> sorted_particles;
		sorted_particles.reserve(pass_data->cpu_particles.size());
		for (size_t i = 0; i < pass_data->cpu_particles.size(); i++)
		{
			Vec3f delta = pass_data->cpu_particles[i].position - eye_pos;
			sorted_particles.push_back(ParticleOrderIndex(i, Vec3f::dot(delta, delta)));
		}
		std::sort(sorted_particles.begin(), sorted_particles.end());

		for (size_t k = 0; k < sorted_particles.size(); k++)
		{
			int i = sorted_particles[k].index;
			float size = mix(pass_data->cpu_particles[i].start_size, pass_data->cpu_particles[i].end_size, pass_data->cpu_particles[i].life);
			vectors[vector_offset + k * vectors_per_particle + 0] = Vec4f(pass_data->cpu_particles[i].position, size);
			vectors[vector_offset + k * vectors_per_particle + 1] = Vec4f(pass_data->cpu_particles[i].life, 0.0f, 0.0f, 0.0f);
		}

		vector_offset += pass_data->cpu_particles.size() * vectors_per_particle;
	}
	instance_transfer->unlock();
	instance_texture->set_image(inout.gc, instance_transfer);

	inout.gc->set_depth_range(0.0f, 0.9f);

	inout.gc->set_frame_buffer(inout.engine->render.frames.front()->current_pipeline_fb());
	inout.gc->set_viewport(viewport_size, inout.gc->texture_image_y_axis());
	inout.gc->set_depth_stencil_state(depth_stencil_state);
	inout.gc->set_blend_state(blend_state);
	inout.gc->set_rasterizer_state(rasterizer_state);
	inout.gc->set_primitives_array(prim_array);

	inout.gc->set_program_object(program);
	inout.gc->set_texture(0, inout.engine->render.frames.front()->face_normal_z_gbuffer);
	inout.gc->set_texture(1, instance_texture);

	for (int slot : active_emitters)
	{
		auto pass_data = &emitter_slots[slot];
		if (pass_data->cpu_particles.empty())
			continue;

		inout.gc->set_uniform_buffer(0, pass_data->gpu_uniforms);
		inout.gc->set_texture(2, pass_data->particle_animation.get());
		inout.gc->set_texture(3, pass_data->life_color_gradient.get());
		inout.gc->draw_primitives_array_instanced(type_triangles, 0, 6, pass_data->cpu_particles.size());
	}

	inout.gc->reset_primitives_array();
	inout.gc->reset_rasterizer_state();
	inout.gc->reset_depth_stencil_state();
	inout.gc->reset_program_object();
	inout.gc->reset_primitives_elements();
	inout.gc->reset_texture(0);
	inout.gc->reset_texture(1);
	inout.gc->reset_texture(2);
	inout.gc->reset_texture(3);
	inout.gc->reset_uniform_buffer(0);
	inout.gc->reset_frame_buffer();

	inout.gc->set_depth_range(0.0f, 1.0f);
}

void ParticleEmitterPass::setup()
{
	if (!program)
	{
		auto vertex_shader = ShaderObject::create(inout.gc, ShaderType::vertex, inout.gc->shader_language() == shader_hlsl ? vertex_particle_emitter_hlsl() : vertex_particle_emitter_glsl());
		vertex_shader->compile();

		auto fragment_shader = ShaderObject::create(inout.gc, ShaderType::fragment, inout.gc->shader_language() == shader_hlsl ? fragment_particle_emitter_hlsl() : fragment_particle_emitter_glsl());
		fragment_shader->compile();

		program = ProgramObject::create(inout.gc);
		program->attach(vertex_shader);
		program->attach(fragment_shader);
		program->bind_attribute_location(0, "AttrPosition");
		program->bind_frag_data_location(0, "FragColor");
		if (!program->try_link())
			throw Exception(string_format("Particle emitter program failed to link: %1", program->info_log()));
		program->set_uniform_buffer_index("Uniforms", 0);
		program->set_uniform1i("FaceNormalZTexture", 0);
		program->set_uniform1i("InstanceTexture", 1);
		program->set_uniform1i("ParticleTexture", 2);
		program->set_uniform1i("ParticleSampler", 2);
		program->set_uniform1i("ColorGradientTexture", 3);
		program->set_uniform1i("ColorGradientSampler", 3);

		billboard_positions = VertexArrayVector<Vec3f>(inout.gc, cpu_billboard_positions, 6);

		BlendStateDescription blend_desc;
		blend_desc.enable_blending(true);
		//blend_desc.set_blend_function(blend_src_alpha, blend_one_minus_src_alpha, blend_zero, blend_zero);
		blend_desc.set_blend_function(blend_one, blend_one, blend_zero, blend_one);
		blend_state = inout.gc->create_blend_state(blend_desc);

		DepthStencilStateDescription depth_stencil_desc;
		depth_stencil_desc.enable_depth_write(false);
		depth_stencil_desc.enable_depth_test(true);
		depth_stencil_desc.set_depth_compare_function(compare_lequal);
		depth_stencil_state = inout.gc->create_depth_stencil_state(depth_stencil_desc);

		RasterizerStateDescription rasterizer_desc;
		rasterizer_desc.set_culled(false);
		rasterizer_state = inout.gc->create_rasterizer_state(rasterizer_desc);

		prim_array = PrimitivesArray::create(inout.gc);
		prim_array->set_attributes(0, billboard_positions);
	}
}

void ParticleEmitterPass::update()
{
	size_t i = 0;
	while (i != active_emitters.size())
	{
		int slot = active_emitters[i];
		auto pass_data = &emitter_slots[slot];
		bool active = pass_data->update(inout.time_elapsed);
		if (!active)
		{
			if (pass_data->emitter)
				pass_data->emitter->pass_data = nullptr;

			active_emitters.erase(active_emitters.begin() + i);
			free_emitters.push_back(slot);
		}
		else
		{
			i++;
		}
	}
}

Vec3f ParticleEmitterPass::cpu_billboard_positions[6] = 
{
	Vec3f(-1.0f, -1.0f, 0.0f),
	Vec3f( 1.0f, -1.0f, 0.0f),
	Vec3f(-1.0f,  1.0f, 0.0f),
	Vec3f(-1.0f,  1.0f, 0.0f),
	Vec3f( 1.0f, -1.0f, 0.0f),
	Vec3f( 1.0f,  1.0f, 0.0f)
};
