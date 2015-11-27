
#include "precomp.h"
#include "model_mesh.h"
#include "Scene3D/ModelData/model_data.h"
#include "Scene3D/SceneEngine/Model/model.h"

using namespace uicore;

ModelMesh::ModelMesh(SceneEngineImpl *engine, const GraphicContextPtr &gc, int model_index, std::shared_ptr<ModelData> model_data) : model_index(model_index), model_data(model_data)
{
/*
	// Small hack until we support rendering unskinned meshes
	if (model_data->bones.empty())
	{
		model_data->bones.resize(1);
		model_data->bones[0].position.timelines.resize(model_data->animations.size());
		model_data->bones[0].orientation.timelines.resize(model_data->animations.size());
		model_data->bones[0].scale.timelines.resize(model_data->animations.size());
		for (size_t i = 0; i < model_data->animations.size(); i++)
		{
			model_data->bones[0].position.timelines[i].timestamps.resize(1);
			model_data->bones[0].position.timelines[i].values.resize(1);
			model_data->bones[0].orientation.timelines[i].timestamps.resize(1);
			model_data->bones[0].orientation.timelines[i].values.resize(1);
			model_data->bones[0].scale.timelines[i].timestamps.resize(1);
			model_data->bones[0].scale.timelines[i].values.resize(1, Vec3f(1.0f));
		}
	}
*/

	mesh_buffers.reserve(model_data->meshes.size());
	for (size_t i = 0; i < model_data->meshes.size(); i++)
	{
		ModelMeshBuffers buffers;
		buffers.primitives_array = PrimitivesArray::create(gc);
		buffers.vertices = upload_vector(gc, buffers.primitives_array, 0, model_data->meshes[i].vertices);
		buffers.normals = upload_vector(gc, buffers.primitives_array, 1, model_data->meshes[i].normals);
		buffers.bitangents = upload_vector(gc, buffers.primitives_array, 2, model_data->meshes[i].bitangents);
		buffers.tangents = upload_vector(gc, buffers.primitives_array, 3, model_data->meshes[i].tangents);
		buffers.bone_weights = upload_vector(gc, buffers.primitives_array, 4, model_data->meshes[i].bone_weights, true);
		buffers.bone_selectors = upload_vector(gc, buffers.primitives_array, 5, model_data->meshes[i].bone_selectors, false);
		buffers.colors = upload_vector(gc, buffers.primitives_array, 6, model_data->meshes[i].colors, true);

		for (size_t channel = 0; channel < model_data->meshes[i].channels.size(); channel++)
		{
			buffers.channels.push_back(upload_vector(gc, buffers.primitives_array, 7 + channel, model_data->meshes[i].channels[channel]));
		}

		buffers.elements = ElementArrayVector<unsigned int>(gc, model_data->meshes[i].elements);

		size_t num_materials = model_data->meshes[i].draw_ranges.size();
		for (size_t j = 0; j < num_materials; j++)
		{
			const ModelDataDrawRange &material_range = model_data->meshes[i].draw_ranges[j];

			ModelMaterialUniforms block;

			block.material_ambient = Vec4f(material_range.ambient.get_single_value(), 0.0f);
			block.material_diffuse = Vec4f(material_range.diffuse.get_single_value(), 0.0f);
			block.material_specular = Vec4f(material_range.specular.get_single_value(), 0.0f);
			block.material_glossiness = material_range.glossiness.get_single_value();
			block.material_specular_level = material_range.specular_level.get_single_value();

			block.model_index = model_index;
			block.vectors_per_instance = instance_base_vectors + model_data->bones.size() * vectors_per_bone + num_materials * vectors_per_material;
			block.material_offset = instance_base_vectors + model_data->bones.size() * vectors_per_bone + j * vectors_per_material;

			buffers.uniforms.push_back(UniformVector<ModelMaterialUniforms>(gc, &block, 1));
		}

		mesh_buffers.push_back(buffers);
	}

	for (size_t i = 0; i < model_data->textures.size(); i++)
		textures.push_back(engine->get_texture(gc, model_data->textures[i].name, model_data->textures[i].gamma == 1.0f));

	create_gbuffer_commands(gc, &engine->render.model_render.shader_cache);
	create_transparency_commands(gc, &engine->render.model_render.shader_cache);
	create_shadow_commands(gc, &engine->render.model_render.shader_cache);
	create_early_z_commands(gc, &engine->render.model_render.shader_cache);
}

bool ModelMesh::add_instance(int instance_frame, const ModelInstance &instance, const Mat4f &object_to_world, const Vec3f &light_probe_color)
{
	if (instance.cur_anim.get_animation_index() == -1)
		return false;

	if (instance_frame != frame)
	{
		frame = instance_frame;
		instances.clear();
		instances_object_to_world.clear();
		instances_light_probe_color.clear();
	}

	instances.push_back(&instance);
	instances_object_to_world.push_back(object_to_world);
	instances_light_probe_color.push_back(light_probe_color);
	return instances.size() == 1;
}

int ModelMesh::get_instance_vectors_count() const
{
	return instances.size() * get_vectors_per_instance();
}

int ModelMesh::get_vectors_per_instance() const
{
	if (model_data->meshes.size() != 1)
		throw Exception("Model::get_vectors_per_instance does not support multiple meshes yet");
	size_t num_materials = model_data->meshes[0].draw_ranges.size();
	return instance_base_vectors + model_data->bones.size() * vectors_per_bone + num_materials * vectors_per_material;
}

void ModelMesh::upload(ModelInstancesBuffer &model_instances_buffer, const Mat4f &world_to_eye, const Mat4f &eye_to_projection)
{
	int vectors_per_instance = get_vectors_per_instance();

	MappedBuffer<Vec4f> instance_data = model_instances_buffer.upload(model_index, get_instance_vectors_count());
	for (size_t j = 0; j < instances.size(); j++)
	{
		MappedBuffer<Vec4f> vectors = instance_data + j * vectors_per_instance;

		Mat3f object_normal_to_eye = Mat3f(world_to_eye * instances_object_to_world[j]).inverse().transpose();

		// mat3 object_normal_to_eye
		vectors[0] = Vec4f(object_normal_to_eye[0], object_normal_to_eye[3], object_normal_to_eye[6], 0.0f);
		vectors[1] = Vec4f(object_normal_to_eye[1], object_normal_to_eye[4], object_normal_to_eye[7], 0.0f);
		vectors[2] = Vec4f(object_normal_to_eye[2], object_normal_to_eye[5], object_normal_to_eye[8], 0.0f);

		// mat4 object_to_world
		vectors[3] = Vec4f(instances_object_to_world[j][0], instances_object_to_world[j][4], instances_object_to_world[j][8], instances_object_to_world[j][12]);
		vectors[4] = Vec4f(instances_object_to_world[j][1], instances_object_to_world[j][5], instances_object_to_world[j][9], instances_object_to_world[j][13]);
		vectors[5] = Vec4f(instances_object_to_world[j][2], instances_object_to_world[j][6], instances_object_to_world[j][10], instances_object_to_world[j][14]);
		vectors[6] = Vec4f(instances_object_to_world[j][3], instances_object_to_world[j][7], instances_object_to_world[j][11], instances_object_to_world[j][15]);

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

		vectors[15] = Vec4f(instances_light_probe_color[j], 0.0f);

		for (size_t i = 0; i < model_data->bones.size(); i++)
		{
			Vec3f position;
			Quaternionf orientation;
			instances[j]->get_bone_transform(i, model_data->bones[i], position, orientation);

			if (model_data->bones[i].billboarded)
			{
				// To do: optimize this away by feeding upload with the camera orientation and the instance orientation
				Vec3f camera_pos, camera_scale;
				Quaternionf camera_orientation;

				Mat4f object_to_eye = world_to_eye * instances_object_to_world[j];
				object_to_eye.decompose(camera_pos, camera_orientation, camera_scale);

				orientation = Quaternionf::inverse(camera_orientation) * orientation;
			}

			Mat4f transform = Mat4f::translate(position) * orientation.to_matrix() * Mat4f::translate(-model_data->bones[i].pivot);
			transform.transpose();
			vectors[instance_base_vectors + i * vectors_per_bone + 0] = Vec4f(transform[0], transform[1], transform[2], transform[3]);
			vectors[instance_base_vectors + i * vectors_per_bone + 1] = Vec4f(transform[4], transform[5], transform[6], transform[7]);
			vectors[instance_base_vectors + i * vectors_per_bone + 2] = Vec4f(transform[8], transform[9], transform[10], transform[11]);
		}

		size_t num_materials = model_data->meshes[0].draw_ranges.size();
		for (size_t i = 0; i < num_materials; i++)
		{
			int material_offset = instance_base_vectors + model_data->bones.size() * vectors_per_bone + i * vectors_per_material;

			int animation_index = instances[j]->cur_anim.get_animation_index();
			float animation_time = instances[j]->cur_anim.get_animation_time();

			Vec3f self_illumination = model_data->meshes[0].draw_ranges[i].self_illumination.get_value(animation_index, animation_time);
			float self_illumination_amount = model_data->meshes[0].draw_ranges[i].self_illumination_amount.get_value(animation_index, animation_time);
			vectors[material_offset + 0] = Vec4f(self_illumination, self_illumination_amount);
			vectors[material_offset + 1] = Vec4f(0.0f); // model_data->meshes[0].draw_ranges[i].diffuse_map.replaceable_texture

			Mat4f uvw[4] =
			{
				model_data->meshes[0].draw_ranges[i].diffuse_map.get_uvw_matrix(animation_index, animation_time),
				model_data->meshes[0].draw_ranges[i].bumpmap_map.get_uvw_matrix(animation_index, animation_time),
				model_data->meshes[0].draw_ranges[i].self_illumination_map.get_uvw_matrix(animation_index, animation_time),
				model_data->meshes[0].draw_ranges[i].specular_map.get_uvw_matrix(animation_index, animation_time)
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
	}
}

template<typename Type>
VertexArrayVector<Type> ModelMesh::upload_vector(const GraphicContextPtr &gc, const PrimitivesArrayPtr &primitives_array, int index, const std::vector<Type> &vec)
{
	if (!vec.empty())
	{
		VertexArrayVector<Type> buffer(gc, vec);
		primitives_array->set_attributes(index, buffer);
		return buffer;
	}
	else
	{
		return VertexArrayVector<Type>();
	}
}

template<typename Type>
VertexArrayVector<Type> ModelMesh::upload_vector(const GraphicContextPtr &gc, const PrimitivesArrayPtr &primitives_array, int index, const std::vector<Type> &vec, bool normalize)
{
	if (!vec.empty())
	{
		VertexArrayVector<Type> buffer(gc, vec);
		primitives_array->set_attributes(index, buffer, normalize);
		return buffer;
	}
	else
	{
		return VertexArrayVector<Type>();
	}
}

void ModelMesh::create_gbuffer_commands(const GraphicContextPtr &gc, ModelShaderCache *shader_cache)
{
	shader_cache->create_states(gc);

	ModelRenderCommandList &out_list = gbuffer_commands;

	bool is_two_sided = false;
	if (!model_data->meshes.empty() && !model_data->meshes[0].draw_ranges.empty())
	{
		is_two_sided = model_data->meshes[0].draw_ranges[0].two_sided;
		out_list.commands.push_back(new ModelRenderCommand_SetRasterizerStatePtr(is_two_sided ? shader_cache->two_sided_rasterizer_state : shader_cache->rasterizer_state));
	}

	bool uses_bones = !model_data->bones.empty();

	bool first_program = true;
	ModelShaderDescription current_program;

	for (size_t i = 0; i < mesh_buffers.size(); i++)
	{
		bool uses_color_channel = (bool)mesh_buffers[i].colors.buffer();

		out_list.commands.push_back(new ModelRenderCommand_BindMeshBuffers(&mesh_buffers[i]));
		for (size_t j = 0; j < model_data->meshes[i].draw_ranges.size(); j++)
		{
			const ModelDataDrawRange &material_range = model_data->meshes[i].draw_ranges[j];

			if (material_range.num_elements == 0) // Some formats got some silly empty material ranges
				continue;

			if (material_range.transparent)
				continue;

			if (material_range.diffuse_map.texture != -1)
			{
				out_list.commands.push_back(new ModelRenderCommand_BindTexture(2, textures[material_range.diffuse_map.texture], to_wrap_mode(material_range.diffuse_map.wrap_x), to_wrap_mode(material_range.diffuse_map.wrap_y)));
			}

			if (material_range.bumpmap_map.texture != -1)
			{
				out_list.commands.push_back(new ModelRenderCommand_BindTexture(3, textures[material_range.bumpmap_map.texture], to_wrap_mode(material_range.bumpmap_map.wrap_x), to_wrap_mode(material_range.bumpmap_map.wrap_y)));
			}

			if (material_range.self_illumination_map.texture != -1)
			{
				out_list.commands.push_back(new ModelRenderCommand_BindTexture(4, textures[material_range.self_illumination_map.texture], to_wrap_mode(material_range.self_illumination_map.wrap_x), to_wrap_mode(material_range.self_illumination_map.wrap_y)));
			}

			if (material_range.specular_map.texture != -1)
			{
				out_list.commands.push_back(new ModelRenderCommand_BindTexture(5, textures[material_range.specular_map.texture], to_wrap_mode(material_range.specular_map.wrap_x), to_wrap_mode(material_range.specular_map.wrap_y)));
			}

			if (material_range.light_map.texture != -1)
			{
				out_list.commands.push_back(new ModelRenderCommand_BindTexture(6, textures[material_range.light_map.texture], to_wrap_mode(material_range.light_map.wrap_x), to_wrap_mode(material_range.light_map.wrap_y)));
			}

			if (is_two_sided != material_range.two_sided)
			{
				is_two_sided = material_range.two_sided;
				out_list.commands.push_back(new ModelRenderCommand_SetRasterizerStatePtr(is_two_sided ? shader_cache->two_sided_rasterizer_state : shader_cache->rasterizer_state));
			}

			ModelShaderDescription shader_desc(material_range, uses_bones, uses_color_channel);
			if (first_program || !(current_program < shader_desc || shader_desc < current_program))
			{
				//first_program = false; // To do: fix bug in clanlib causing resource binds to be incorrect in some situations if we do not bind the program every time
				current_program = shader_desc;
				out_list.commands.push_back(new ModelRenderCommand_BindShader(shader_cache->get_shader(gc, shader_desc).gbuffer));
			}

			out_list.commands.push_back(new ModelRenderCommand_DrawElements(material_range.start_element, material_range.num_elements, mesh_buffers[i].uniforms[j]));
		}
	}
}

void ModelMesh::create_transparency_commands(const GraphicContextPtr &gc, ModelShaderCache *shader_cache)
{
	shader_cache->create_states(gc);

	ModelRenderCommandList &out_list = transparency_commands;

	bool is_two_sided = false;
	if (!model_data->meshes.empty() && !model_data->meshes[0].draw_ranges.empty())
	{
		is_two_sided = model_data->meshes[0].draw_ranges[0].two_sided;
	}

	bool uses_bones = !model_data->bones.empty();

	bool raster_set = false;
	bool first_program = true;
	ModelShaderDescription current_program;

	for (size_t i = 0; i < mesh_buffers.size(); i++)
	{
		bool uses_color_channel = (bool)mesh_buffers[i].colors.buffer();
		bool buffers_set = false;

		for (size_t j = 0; j < model_data->meshes[i].draw_ranges.size(); j++)
		{
			const ModelDataDrawRange &material_range = model_data->meshes[i].draw_ranges[j];

			if (material_range.num_elements == 0) // Some formats got some silly empty material ranges
				continue;

			if (!material_range.transparent)
				continue;

			if (!raster_set)
			{
				out_list.commands.push_back(new ModelRenderCommand_SetRasterizerStatePtr(is_two_sided ? shader_cache->two_sided_rasterizer_state : shader_cache->rasterizer_state));
				raster_set = true;
			}

			if (!buffers_set)
			{
				out_list.commands.push_back(new ModelRenderCommand_BindMeshBuffers(&mesh_buffers[i]));
				buffers_set = true;
			}

			if (material_range.diffuse_map.texture != -1)
			{
				out_list.commands.push_back(new ModelRenderCommand_BindTexture(2, textures[material_range.diffuse_map.texture], to_wrap_mode(material_range.diffuse_map.wrap_x), to_wrap_mode(material_range.diffuse_map.wrap_y)));
			}

			if (material_range.bumpmap_map.texture != -1)
			{
				out_list.commands.push_back(new ModelRenderCommand_BindTexture(3, textures[material_range.bumpmap_map.texture], to_wrap_mode(material_range.bumpmap_map.wrap_x), to_wrap_mode(material_range.bumpmap_map.wrap_y)));
			}

			if (material_range.self_illumination_map.texture != -1)
			{
				out_list.commands.push_back(new ModelRenderCommand_BindTexture(4, textures[material_range.self_illumination_map.texture], to_wrap_mode(material_range.self_illumination_map.wrap_x), to_wrap_mode(material_range.self_illumination_map.wrap_y)));
			}

			if (material_range.specular_map.texture != -1)
			{
				out_list.commands.push_back(new ModelRenderCommand_BindTexture(5, textures[material_range.specular_map.texture], to_wrap_mode(material_range.specular_map.wrap_x), to_wrap_mode(material_range.specular_map.wrap_y)));
			}

			if (material_range.light_map.texture != -1)
			{
				out_list.commands.push_back(new ModelRenderCommand_BindTexture(6, textures[material_range.light_map.texture], to_wrap_mode(material_range.light_map.wrap_x), to_wrap_mode(material_range.light_map.wrap_y)));
			}

			if (is_two_sided != material_range.two_sided)
			{
				is_two_sided = material_range.two_sided;
				out_list.commands.push_back(new ModelRenderCommand_SetRasterizerStatePtr(is_two_sided ? shader_cache->two_sided_rasterizer_state : shader_cache->rasterizer_state));
			}

			ModelShaderDescription shader_desc(material_range, uses_bones, uses_color_channel);
			if (first_program || !(current_program < shader_desc || shader_desc < current_program))
			{
				//first_program = false; // To do: fix bug in clanlib causing resource binds to be incorrect in some situations if we do not bind the program every time
				current_program = shader_desc;
				out_list.commands.push_back(new ModelRenderCommand_BindShader(shader_cache->get_shader(gc, shader_desc).transparency));
			}

			out_list.commands.push_back(new ModelRenderCommand_DrawElements(material_range.start_element, material_range.num_elements, mesh_buffers[i].uniforms[j]));
		}
	}
}

void ModelMesh::create_shadow_commands(const GraphicContextPtr &gc, ModelShaderCache *shader_cache)
{
	shader_cache->create_states(gc);

	ModelRenderCommandList &out_list = shadow_commands;

	// To do: add support for two_sided

	bool uses_bones = !model_data->bones.empty();

	out_list.commands.push_back(new ModelRenderCommand_SetRasterizerStatePtr(shader_cache->rasterizer_state));
	out_list.commands.push_back(new ModelRenderCommand_BindShader(shader_cache->get_shadow_program(gc, uses_bones)));
	for (size_t i = 0; i < mesh_buffers.size(); i++)
	{
		out_list.commands.push_back(new ModelRenderCommand_BindMeshBuffers(&mesh_buffers[i]));

		ModelDataMesh &mesh_data = model_data->meshes[i];
		out_list.commands.push_back(new ModelRenderCommand_DrawElements(0, mesh_data.draw_ranges.back().start_element + mesh_data.draw_ranges.back().num_elements, mesh_buffers[i].uniforms[0]));
	}
}

void ModelMesh::create_early_z_commands(const GraphicContextPtr &gc, ModelShaderCache *shader_cache)
{
	shader_cache->create_states(gc);

	ModelRenderCommandList &out_list = early_z_commands;

	// To do: add support for two_sided

	bool uses_bones = !model_data->bones.empty();

	out_list.commands.push_back(new ModelRenderCommand_SetRasterizerStatePtr(shader_cache->rasterizer_state));
	out_list.commands.push_back(new ModelRenderCommand_BindShader(shader_cache->get_early_z_program(gc, uses_bones)));
	for (size_t i = 0; i < mesh_buffers.size(); i++)
	{
		ModelDataMesh &mesh_data = model_data->meshes[i];

		size_t compatible_ranges_count = 0;
		for (size_t j = 0; j < mesh_data.draw_ranges.size(); j++)
		{
			if (!mesh_data.draw_ranges[j].transparent && !mesh_data.draw_ranges[j].two_sided && !mesh_data.draw_ranges[j].alpha_test)
				compatible_ranges_count++;
		}

		if (compatible_ranges_count > 0)
			out_list.commands.push_back(new ModelRenderCommand_BindMeshBuffers(&mesh_buffers[i]));

		if (compatible_ranges_count == mesh_data.draw_ranges.size())
		{
			out_list.commands.push_back(new ModelRenderCommand_DrawElements(0, mesh_data.draw_ranges.back().start_element + mesh_data.draw_ranges.back().num_elements, mesh_buffers[i].uniforms[0]));
		}
		else
		{
			for (size_t j = 0; j < mesh_data.draw_ranges.size(); j++)
			{
				if (!mesh_data.draw_ranges[j].transparent && !mesh_data.draw_ranges[j].two_sided && !mesh_data.draw_ranges[j].alpha_test)
				{
					out_list.commands.push_back(new ModelRenderCommand_DrawElements(mesh_data.draw_ranges[j].start_element, mesh_data.draw_ranges[j].num_elements, mesh_buffers[i].uniforms[j]));
				}
			}
		}
	}
}

TextureWrapMode ModelMesh::to_wrap_mode(ModelDataTextureMap::WrapMode mode)
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
