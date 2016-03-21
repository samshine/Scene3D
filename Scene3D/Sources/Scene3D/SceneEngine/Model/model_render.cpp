
#include "precomp.h"
#include "model_render.h"
#include "model_mesh.h"
#include "Scene3D/ModelData/model_data.h"
#include "Scene3D/Scene/scene_object_impl.h"
#include "Scene3D/Scene/scene_light_probe_impl.h"
#include "Scene3D/Scene/scene_impl.h"
#include "Scene3D/Performance/scope_timer.h"
#include "Scene3D/SceneEngine/Model/model.h"
#include "Scene3D/SceneEngine/Model/model_mesh.h"

using namespace uicore;

ModelRender::ModelRender()
{
}

void ModelRender::begin(SceneImpl *new_scene, const Mat4f &new_world_to_eye, const Mat4f &new_eye_to_projection, ModelRenderMode new_render_mode)
{
	scene = new_scene;
	world_to_eye = new_world_to_eye;
	eye_to_projection = new_eye_to_projection;
	render_mode = new_render_mode;
	render_lists.clear();
	scene->engine()->render.scene_visits++;
}

void ModelRender::render(SceneObjectImpl *object)
{
	if (!object->instance.get_renderer()->mesh)
		object->instance.get_renderer()->create_mesh(scene->engine()->render.gc);

	auto mesh = object->instance.get_renderer()->mesh.get();

	auto &list = render_lists[mesh];
	list.push_back(object);
	if (list.size() >= batch_size)
	{
		render_mesh(mesh, list);
		list.clear();
	}

	scene->engine()->render.instances_drawn++;
}

void ModelRender::end()
{
	for (auto &it : render_lists)
	{
		render_mesh(it.first, it.second);
	}
	render_lists.clear();
}

void ModelRender::render_mesh(ModelMesh *mesh, const std::vector<SceneObjectImpl *> &instances)
{
	ScopeTimeFunction();

	scene->engine()->render.models_drawn++;

	auto instance_buffer = upload_instances(mesh, instances);

	auto &inout = scene->engine()->render;
	shader_cache.create_states(inout.gc);

	switch (render_mode)
	{
	case ModelRenderMode::early_z: render_early_z(mesh, instance_buffer, instances.size()); break;
	case ModelRenderMode::shadow: render_shadow(mesh, instance_buffer, instances.size()); break;
	case ModelRenderMode::gbuffer: render_gbuffer(mesh, instance_buffer, instances.size()); break;
	case ModelRenderMode::transparency: render_transparency(mesh, instance_buffer, instances.size()); break;
	}

	inout.gc->reset_texture(0);
}

uicore::Texture2DPtr ModelRender::upload_instances(ModelMesh *mesh, const std::vector<SceneObjectImpl *> &instances)
{
	ScopeTimeFunction();

	auto &inout = scene->engine()->render;

	auto vectors_per_instance = mesh->vectors_per_instance();

	auto staging_buffer = get_staging_buffer(instances.size() * vectors_per_instance);
	staging_buffer->lock(inout.gc, access_write_discard);
	auto vectors = staging_buffer->data<Vec4f>();

	for (const auto &object : instances)
	{
		Vec3f light_probe_color;
		if (object->light_probe_receiver())
		{
			SceneLightProbeImpl *probe = find_nearest_probe(object->position());
			if (probe)
				light_probe_color = probe->color();
		}

		const auto &object_to_world = object->get_object_to_world();

		Mat3f object_normal_to_eye = Mat3f(world_to_eye * object_to_world).inverse().transpose();

		// mat3 object_normal_to_eye
		vectors[0] = Vec4f(object_normal_to_eye[0], object_normal_to_eye[3], object_normal_to_eye[6], 0.0f);
		vectors[1] = Vec4f(object_normal_to_eye[1], object_normal_to_eye[4], object_normal_to_eye[7], 0.0f);
		vectors[2] = Vec4f(object_normal_to_eye[2], object_normal_to_eye[5], object_normal_to_eye[8], 0.0f);

		// mat4 object_to_world
		vectors[3] = Vec4f(object_to_world[0], object_to_world[4], object_to_world[8], object_to_world[12]);
		vectors[4] = Vec4f(object_to_world[1], object_to_world[5], object_to_world[9], object_to_world[13]);
		vectors[5] = Vec4f(object_to_world[2], object_to_world[6], object_to_world[10], object_to_world[14]);
		vectors[6] = Vec4f(object_to_world[3], object_to_world[7], object_to_world[11], object_to_world[15]);

		// mat4 world_to_eye
		vectors[7] = Vec4f(world_to_eye[0], world_to_eye[4], world_to_eye[8], world_to_eye[12]);
		vectors[8] = Vec4f(world_to_eye[1], world_to_eye[5], world_to_eye[9], world_to_eye[13]);
		vectors[9] = Vec4f(world_to_eye[2], world_to_eye[6], world_to_eye[10], world_to_eye[14]);
		vectors[10] = Vec4f(world_to_eye[3], world_to_eye[7], world_to_eye[11], world_to_eye[15]);

		// mat4 eye_to_projection
		vectors[11] = Vec4f(eye_to_projection[0], eye_to_projection[4], eye_to_projection[8], eye_to_projection[12]);
		vectors[12] = Vec4f(eye_to_projection[1], eye_to_projection[5], eye_to_projection[9], eye_to_projection[13]);
		vectors[13] = Vec4f(eye_to_projection[2], eye_to_projection[6], eye_to_projection[10], eye_to_projection[14]);
		vectors[14] = Vec4f(eye_to_projection[3], eye_to_projection[7], eye_to_projection[11], eye_to_projection[15]);

		vectors[15] = Vec4f(light_probe_color, 0.0f);

		for (size_t i = 0; i < mesh->model_data->bones.size(); i++)
		{
			Vec3f position;
			Quaternionf orientation;
			object->instance.get_bone_transform(i, mesh->model_data->bones[i], position, orientation);

			if (mesh->model_data->bones[i].billboarded)
			{
				// To do: optimize this away by feeding upload with the camera orientation and the instance orientation
				Vec3f camera_pos, camera_scale;
				Quaternionf camera_orientation;

				Mat4f object_to_eye = world_to_eye * object_to_world;
				object_to_eye.decompose(camera_pos, camera_orientation, camera_scale);

				orientation = Quaternionf::inverse(camera_orientation) * orientation;
			}

			Mat4f transform = Mat4f::translate(position) * orientation.to_matrix() * Mat4f::translate(-mesh->model_data->bones[i].pivot);
			transform.transpose();
			vectors[mesh->instance_base_vectors + i * mesh->vectors_per_bone + 0] = Vec4f(transform[0], transform[1], transform[2], transform[3]);
			vectors[mesh->instance_base_vectors + i * mesh->vectors_per_bone + 1] = Vec4f(transform[4], transform[5], transform[6], transform[7]);
			vectors[mesh->instance_base_vectors + i * mesh->vectors_per_bone + 2] = Vec4f(transform[8], transform[9], transform[10], transform[11]);
		}

		size_t num_materials = mesh->model_data->meshes[0].draw_ranges.size();
		for (size_t i = 0; i < num_materials; i++)
		{
			int material_offset = mesh->instance_base_vectors + mesh->model_data->bones.size() * mesh->vectors_per_bone + i * mesh->vectors_per_material;

			int animation_index = object->instance.cur_anim.get_animation_index();
			float animation_time = object->instance.cur_anim.get_animation_time();

			Vec3f self_illumination = mesh->model_data->meshes[0].draw_ranges[i].self_illumination.get_value(animation_index, animation_time);
			float self_illumination_amount = mesh->model_data->meshes[0].draw_ranges[i].self_illumination_amount.get_value(animation_index, animation_time);
			vectors[material_offset + 0] = Vec4f(self_illumination, self_illumination_amount);
			vectors[material_offset + 1] = Vec4f(0.0f); // mesh->model_data->meshes[0].draw_ranges[i].diffuse_map.replaceable_texture

			Mat4f uvw[4] =
			{
				mesh->model_data->meshes[0].draw_ranges[i].diffuse_map.get_uvw_matrix(animation_index, animation_time),
				mesh->model_data->meshes[0].draw_ranges[i].bumpmap_map.get_uvw_matrix(animation_index, animation_time),
				mesh->model_data->meshes[0].draw_ranges[i].self_illumination_map.get_uvw_matrix(animation_index, animation_time),
				mesh->model_data->meshes[0].draw_ranges[i].specular_map.get_uvw_matrix(animation_index, animation_time)
			};

			for (int h = 0; h < 4; h++)
				uvw[h].transpose();

			for (int h = 0; h < 4; h++)
			{
				vectors[material_offset + 2 + h * 3 + 0] = Vec4f(uvw[h][0], uvw[h][1], uvw[h][2], uvw[h][3]);
				vectors[material_offset + 2 + h * 3 + 1] = Vec4f(uvw[h][4], uvw[h][5], uvw[h][6], uvw[h][7]);
				vectors[material_offset + 2 + h * 3 + 2] = Vec4f(uvw[h][8], uvw[h][9], uvw[h][10], uvw[h][11]);
			}
		}

		vectors += vectors_per_instance;
	}

	staging_buffer->unlock();

	auto instance_buffer = get_instance_buffer(instances.size() * vectors_per_instance);
	instance_buffer->set_subimage(inout.gc, 0, 0, staging_buffer, staging_buffer->size());
	return instance_buffer;
}

SceneLightProbeImpl *ModelRender::find_nearest_probe(const Vec3f &position)
{
	SceneLightProbeImpl *probe = 0;
	float sqr_distance = 0.0f;

	scene->foreach_light_probe(position, [&](SceneLightProbeImpl *current_probe)
	{
		Vec3f delta = current_probe->position() - position;
		float current_sqr_distance = Vec3f::dot(delta, delta);
		if (probe == 0 || current_sqr_distance < sqr_distance)
		{
			probe = current_probe;
			sqr_distance = current_sqr_distance;
		}
	});

	return probe;
}

Texture2DPtr ModelRender::get_instance_buffer(int size)
{
	auto &inout = scene->engine()->render;
	auto &instance_buffers = inout.frames.front()->model_instance_buffers;
	auto &next = inout.frames.front()->next_model_instance_buffer;

	if (instance_buffers.size() <= next)
		instance_buffers.resize(next + 1);
	auto &buffer = instance_buffers[next];

	auto height_needed = std::max((size + 255) / 256, 1);
	if (!buffer || buffer->size().height < height_needed)
		buffer = Texture2D::create(scene->engine()->render.gc, 256, height_needed, tf_rgba32f);

	next++;
	return buffer;
}

StagingTexturePtr ModelRender::get_staging_buffer(int size)
{
	auto &inout = scene->engine()->render;
	auto &staging_buffers = inout.frames.front()->model_staging_buffers;
	auto &next = inout.frames.front()->next_model_staging_buffer;

	if (staging_buffers.size() <= next)
		staging_buffers.resize(next + 1);
	auto &buffer = staging_buffers[next];

	auto height_needed = std::max((size + 255) / 256, 1);
	if (!buffer || buffer->size().height < height_needed)
		buffer = StagingTexture::create(scene->engine()->render.gc, 256, height_needed, StagingDirection::to_gpu, tf_rgba32f);

	next++;
	return buffer;
}

void ModelRender::render_early_z(ModelMesh *mesh, uicore::Texture2DPtr instance_buffer, int instance_count)
{
	auto &inout = scene->engine()->render;

	// To do: add support for two_sided

	bool uses_bones = !mesh->model_data->bones.empty();

	inout.gc->set_rasterizer_state(shader_cache.rasterizer_state);
	inout.gc->set_program_object(shader_cache.get_early_z_program(inout.gc, uses_bones));
	for (size_t i = 0; i < mesh->mesh_buffers.size(); i++)
	{
		ModelDataMesh &mesh_data = mesh->model_data->meshes[i];

		size_t compatible_ranges_count = 0;
		for (size_t j = 0; j < mesh_data.draw_ranges.size(); j++)
		{
			if (!mesh_data.draw_ranges[j].transparent && !mesh_data.draw_ranges[j].two_sided && !mesh_data.draw_ranges[j].alpha_test)
				compatible_ranges_count++;
		}

		if (compatible_ranges_count > 0)
		{
			inout.gc->set_primitives_array(mesh->mesh_buffers[i].primitives_array);
			inout.gc->set_primitives_elements(mesh->mesh_buffers[i].elements);
		}

		if (compatible_ranges_count == mesh_data.draw_ranges.size())
		{
			inout.gc->set_texture(0, instance_buffer);
			draw_elements(0, mesh_data.draw_ranges.back().start_element + mesh_data.draw_ranges.back().num_elements, mesh->mesh_buffers[i].uniforms[0], instance_count);
		}
		else
		{
			for (size_t j = 0; j < mesh_data.draw_ranges.size(); j++)
			{
				if (!mesh_data.draw_ranges[j].transparent && !mesh_data.draw_ranges[j].two_sided && !mesh_data.draw_ranges[j].alpha_test)
				{
					inout.gc->set_texture(0, instance_buffer);
					draw_elements(mesh_data.draw_ranges[j].start_element, mesh_data.draw_ranges[j].num_elements, mesh->mesh_buffers[i].uniforms[j], instance_count);
				}
			}
		}
	}
}

void ModelRender::render_shadow(ModelMesh *mesh, uicore::Texture2DPtr instance_buffer, int instance_count)
{
	auto &inout = scene->engine()->render;

	// To do: add support for two_sided

	bool uses_bones = !mesh->model_data->bones.empty();

	inout.gc->set_rasterizer_state(shader_cache.rasterizer_state);
	inout.gc->set_program_object(shader_cache.get_shadow_program(inout.gc, uses_bones));
	for (size_t i = 0; i < mesh->mesh_buffers.size(); i++)
	{
		inout.gc->set_primitives_array(mesh->mesh_buffers[i].primitives_array);
		inout.gc->set_primitives_elements(mesh->mesh_buffers[i].elements);

		ModelDataMesh &mesh_data = mesh->model_data->meshes[i];
		inout.gc->set_texture(0, instance_buffer);
		draw_elements(0, mesh_data.draw_ranges.back().start_element + mesh_data.draw_ranges.back().num_elements, mesh->mesh_buffers[i].uniforms[0], instance_count);
	}
}

void ModelRender::render_gbuffer(ModelMesh *mesh, uicore::Texture2DPtr instance_buffer, int instance_count)
{
	auto &inout = scene->engine()->render;

	bool is_two_sided = false;
	if (!mesh->model_data->meshes.empty() && !mesh->model_data->meshes[0].draw_ranges.empty())
	{
		is_two_sided = mesh->model_data->meshes[0].draw_ranges[0].two_sided;
		inout.gc->set_rasterizer_state(is_two_sided ? shader_cache.two_sided_rasterizer_state : shader_cache.rasterizer_state);
	}

	bool uses_bones = !mesh->model_data->bones.empty();

	bool first_program = true;
	ModelShaderDescription current_program;

	for (size_t i = 0; i < mesh->mesh_buffers.size(); i++)
	{
		bool uses_color_channel = (bool)mesh->mesh_buffers[i].colors.buffer();

		inout.gc->set_primitives_array(mesh->mesh_buffers[i].primitives_array);
		inout.gc->set_primitives_elements(mesh->mesh_buffers[i].elements);

		for (size_t j = 0; j < mesh->model_data->meshes[i].draw_ranges.size(); j++)
		{
			const ModelDataDrawRange &material_range = mesh->model_data->meshes[i].draw_ranges[j];

			if (material_range.num_elements == 0) // Some formats got some silly empty material ranges
				continue;

			if (material_range.transparent)
				continue;

			if (material_range.diffuse_map.texture != -1)
			{
				bind_texture(2, mesh->textures[material_range.diffuse_map.texture], to_wrap_mode(material_range.diffuse_map.wrap_x), to_wrap_mode(material_range.diffuse_map.wrap_y));
			}

			if (material_range.bumpmap_map.texture != -1)
			{
				bind_texture(3, mesh->textures[material_range.bumpmap_map.texture], to_wrap_mode(material_range.bumpmap_map.wrap_x), to_wrap_mode(material_range.bumpmap_map.wrap_y));
			}

			if (material_range.self_illumination_map.texture != -1)
			{
				bind_texture(4, mesh->textures[material_range.self_illumination_map.texture], to_wrap_mode(material_range.self_illumination_map.wrap_x), to_wrap_mode(material_range.self_illumination_map.wrap_y));
			}

			if (material_range.specular_map.texture != -1)
			{
				bind_texture(5, mesh->textures[material_range.specular_map.texture], to_wrap_mode(material_range.specular_map.wrap_x), to_wrap_mode(material_range.specular_map.wrap_y));
			}

			if (material_range.light_map.texture != -1)
			{
				bind_texture(6, mesh->textures[material_range.light_map.texture], to_wrap_mode(material_range.light_map.wrap_x), to_wrap_mode(material_range.light_map.wrap_y));
			}

			if (is_two_sided != material_range.two_sided)
			{
				is_two_sided = material_range.two_sided;
				inout.gc->set_rasterizer_state(is_two_sided ? shader_cache.two_sided_rasterizer_state : shader_cache.rasterizer_state);
			}

			ModelShaderDescription shader_desc(material_range, uses_bones, uses_color_channel);
			if (first_program || !(current_program < shader_desc || shader_desc < current_program))
			{
				//first_program = false; // To do: fix bug in clanlib causing resource binds to be incorrect in some situations if we do not bind the program every time
				current_program = shader_desc;
				inout.gc->set_program_object(shader_cache.get_shader(inout.gc, shader_desc).gbuffer);
			}

			inout.gc->set_texture(0, instance_buffer);
			draw_elements(material_range.start_element, material_range.num_elements, mesh->mesh_buffers[i].uniforms[j], instance_count);
		}
	}
}

void ModelRender::render_transparency(ModelMesh *mesh, uicore::Texture2DPtr instance_buffer, int instance_count)
{
	auto &inout = scene->engine()->render;

	bool is_two_sided = false;
	if (!mesh->model_data->meshes.empty() && !mesh->model_data->meshes[0].draw_ranges.empty())
	{
		is_two_sided = mesh->model_data->meshes[0].draw_ranges[0].two_sided;
	}

	bool uses_bones = !mesh->model_data->bones.empty();

	bool raster_set = false;
	bool first_program = true;
	ModelShaderDescription current_program;

	for (size_t i = 0; i < mesh->mesh_buffers.size(); i++)
	{
		bool uses_color_channel = (bool)mesh->mesh_buffers[i].colors.buffer();
		bool buffers_set = false;

		for (size_t j = 0; j < mesh->model_data->meshes[i].draw_ranges.size(); j++)
		{
			const ModelDataDrawRange &material_range = mesh->model_data->meshes[i].draw_ranges[j];

			if (material_range.num_elements == 0) // Some formats got some silly empty material ranges
				continue;

			if (!material_range.transparent)
				continue;

			if (!raster_set)
			{
				inout.gc->set_rasterizer_state(is_two_sided ? shader_cache.two_sided_rasterizer_state : shader_cache.rasterizer_state);
				raster_set = true;
			}

			if (!buffers_set)
			{
				inout.gc->set_primitives_array(mesh->mesh_buffers[i].primitives_array);
				inout.gc->set_primitives_elements(mesh->mesh_buffers[i].elements);
				buffers_set = true;
			}

			if (material_range.diffuse_map.texture != -1)
			{
				bind_texture(2, mesh->textures[material_range.diffuse_map.texture], to_wrap_mode(material_range.diffuse_map.wrap_x), to_wrap_mode(material_range.diffuse_map.wrap_y));
			}

			if (material_range.bumpmap_map.texture != -1)
			{
				bind_texture(3, mesh->textures[material_range.bumpmap_map.texture], to_wrap_mode(material_range.bumpmap_map.wrap_x), to_wrap_mode(material_range.bumpmap_map.wrap_y));
			}

			if (material_range.self_illumination_map.texture != -1)
			{
				bind_texture(4, mesh->textures[material_range.self_illumination_map.texture], to_wrap_mode(material_range.self_illumination_map.wrap_x), to_wrap_mode(material_range.self_illumination_map.wrap_y));
			}

			if (material_range.specular_map.texture != -1)
			{
				bind_texture(5, mesh->textures[material_range.specular_map.texture], to_wrap_mode(material_range.specular_map.wrap_x), to_wrap_mode(material_range.specular_map.wrap_y));
			}

			if (material_range.light_map.texture != -1)
			{
				bind_texture(6, mesh->textures[material_range.light_map.texture], to_wrap_mode(material_range.light_map.wrap_x), to_wrap_mode(material_range.light_map.wrap_y));
			}

			if (is_two_sided != material_range.two_sided)
			{
				is_two_sided = material_range.two_sided;
				inout.gc->set_rasterizer_state(is_two_sided ? shader_cache.two_sided_rasterizer_state : shader_cache.rasterizer_state);
			}

			ModelShaderDescription shader_desc(material_range, uses_bones, uses_color_channel);
			if (first_program || !(current_program < shader_desc || shader_desc < current_program))
			{
				//first_program = false; // To do: fix bug in clanlib causing resource binds to be incorrect in some situations if we do not bind the program every time
				current_program = shader_desc;
				inout.gc->set_program_object(shader_cache.get_shader(inout.gc, shader_desc).transparency);
			}

			inout.gc->set_texture(0, instance_buffer);
			draw_elements(material_range.start_element, material_range.num_elements, mesh->mesh_buffers[i].uniforms[j], instance_count);
		}
	}
}

void ModelRender::bind_texture(int bind_index, Resource<TexturePtr> texture, TextureWrapMode wrap_u, TextureWrapMode wrap_v)
{
	std::dynamic_pointer_cast<Texture2D>(texture.get())->set_wrap_mode(wrap_u, wrap_v);
	scene->engine()->render.gc->set_texture(bind_index, texture.get());
}

void ModelRender::draw_elements(int start_element, int num_elements, UniformVector<ModelMaterialUniforms> uniforms, int num_instances)
{
	auto &inout = scene->engine()->render;

	inout.gc->set_uniform_buffer(0, uniforms);
	inout.gc->draw_primitives_elements_instanced(type_triangles, num_elements, type_unsigned_int, start_element * sizeof(unsigned int), num_instances);

	scene->engine()->render.draw_calls++;
	scene->engine()->render.triangles_drawn += num_elements / 3;
}

TextureWrapMode ModelRender::to_wrap_mode(ModelDataTextureMap::WrapMode mode)
{
	switch (mode)
	{
	default:
	case ModelDataTextureMap::wrap_repeat:
		return wrap_repeat;
	case ModelDataTextureMap::wrap_clamp_to_edge:
		return wrap_clamp_to_edge;
	case ModelDataTextureMap::wrap_mirror:
		return wrap_mirrored_repeat;
	}
}
