/*
**  ClanLib SDK
**  Copyright (c) 1997-2013 The ClanLib Team
**
**  This software is provided 'as-is', without any express or implied
**  warranty.  In no event will the authors be held liable for any damages
**  arising from the use of this software.
**
**  Permission is granted to anyone to use this software for any purpose,
**  including commercial applications, and to alter it and redistribute it
**  freely, subject to the following restrictions:
**
**  1. The origin of this software must not be misrepresented; you must not
**     claim that you wrote the original software. If you use this software
**     in a product, an acknowledgment in the product documentation would be
**     appreciated but is not required.
**  2. Altered source versions must be plainly marked as such, and must not be
**     misrepresented as being the original software.
**  3. This notice may not be removed or altered from any source distribution.
**
**  Note: Some of the libraries ClanLib may link to may have additional
**  requirements or restrictions.
**
**  File Author(s):
**
**    Magnus Norddahl
*/

#include "precomp.h"
#include "Scene3D/ModelData/model_data.h"

namespace clan
{
	class CModelFormat
	{
	public:
		static void save(clan::IODevice &device, std::shared_ptr<clan::ModelData> data, const std::string &base_path);
		static std::shared_ptr<clan::ModelData> load(const std::string &filename);
		static std::shared_ptr<clan::ModelData> load(clan::IODevice &device, const std::string &base_path);

	private:
		template<typename Type>
		static void write_animation_data(clan::IODevice &file, const clan::ModelDataAnimationData<Type> &animation_data);

		template<typename Type>
		static void write_vector_contents(clan::IODevice &file, const std::vector<Type> &vector_data);

		template<typename Type>
		static void read_animation_data(clan::IODevice &file, clan::ModelDataAnimationData<Type> &out_animation_data);

		template<typename Type>
		static void read_vector_contents(clan::IODevice &file, std::vector<Type> &out_vector_data);
	};
	
	/////////////////////////////////////////////////////////////////////////

	void ModelData::save(IODevice &device, std::shared_ptr<ModelData> data, const std::string &base_path)
	{
		CModelFormat::save(device, data, base_path);
	}

	std::shared_ptr<ModelData> ModelData::load(const std::string &filename)
	{
		return CModelFormat::load(filename);
	}

	std::shared_ptr<ModelData> ModelData::load(IODevice &device, const std::string &base_path)
	{
		return CModelFormat::load(device, base_path);
	}

	/////////////////////////////////////////////////////////////////////////

	inline void CModelFormat::save(clan::IODevice &file, std::shared_ptr<clan::ModelData> data, const std::string &base_path)
	{
		file.write_uint32(13); // version number
		file.write("ModelCaramba", 12); // file magic

		file.write_uint32(data->textures.size());
		file.write_uint32(data->bones.size());
		file.write_uint32(data->lights.size());
		file.write_uint32(data->cameras.size());
		file.write_uint32(data->attachment_points.size());
		file.write_uint32(data->particle_emitters.size());
		file.write_uint32(data->animations.size());
		file.write_float(data->aabb_min.x);
		file.write_float(data->aabb_min.y);
		file.write_float(data->aabb_min.z);
		file.write_float(data->aabb_max.x);
		file.write_float(data->aabb_max.y);
		file.write_float(data->aabb_max.z);

		file.write_uint32(data->meshes.size());
		for (size_t j = 0; j < data->meshes.size(); j++)
		{
			file.write_uint32(data->meshes[j].vertices.size());
			file.write_uint32(data->meshes[j].normals.size());
			file.write_uint32(data->meshes[j].tangents.size());
			file.write_uint32(data->meshes[j].bitangents.size());
			file.write_uint32(data->meshes[j].bone_weights.size());
			file.write_uint32(data->meshes[j].bone_selectors.size());
			file.write_uint32(data->meshes[j].colors.size());
			file.write_uint32(data->meshes[j].channels.size());
			file.write_uint32(data->meshes[j].elements.size());
			file.write_uint32(data->meshes[j].draw_ranges.size());

			write_vector_contents(file, data->meshes[j].vertices);
			write_vector_contents(file, data->meshes[j].normals);
			write_vector_contents(file, data->meshes[j].tangents);
			write_vector_contents(file, data->meshes[j].bitangents);
			write_vector_contents(file, data->meshes[j].bone_weights);
			write_vector_contents(file, data->meshes[j].bone_selectors);
			write_vector_contents(file, data->meshes[j].colors);

			for (size_t k = 0; k < data->meshes[j].channels.size(); k++)
			{
				file.write_uint32(data->meshes[j].channels[k].size());
				write_vector_contents(file, data->meshes[j].channels[k]);
			}

			write_vector_contents(file, data->meshes[j].elements);

			for (size_t k = 0; k < data->meshes[j].draw_ranges.size(); k++)
			{
				file.write_float(data->meshes[j].draw_ranges[k].ambient.get_single_value().x);
				file.write_float(data->meshes[j].draw_ranges[k].ambient.get_single_value().y);
				file.write_float(data->meshes[j].draw_ranges[k].ambient.get_single_value().z);

				file.write_float(data->meshes[j].draw_ranges[k].diffuse.get_single_value().x);
				file.write_float(data->meshes[j].draw_ranges[k].diffuse.get_single_value().y);
				file.write_float(data->meshes[j].draw_ranges[k].diffuse.get_single_value().z);

				file.write_float(data->meshes[j].draw_ranges[k].specular.get_single_value().x);
				file.write_float(data->meshes[j].draw_ranges[k].specular.get_single_value().y);
				file.write_float(data->meshes[j].draw_ranges[k].specular.get_single_value().z);

				file.write_float(data->meshes[j].draw_ranges[k].glossiness.get_single_value());
				file.write_float(data->meshes[j].draw_ranges[k].specular_level.get_single_value());

				write_animation_data(file, data->meshes[j].draw_ranges[k].self_illumination_amount);
				write_animation_data(file, data->meshes[j].draw_ranges[k].self_illumination);

				file.write_uint32(data->meshes[j].draw_ranges[k].two_sided ? 1 : 0);
				file.write_uint32(data->meshes[j].draw_ranges[k].transparent ? 1 : 0);

				file.write_int32(data->meshes[j].draw_ranges[k].diffuse_map.texture);
				file.write_int32(data->meshes[j].draw_ranges[k].diffuse_map.channel);
				file.write_uint32(data->meshes[j].draw_ranges[k].diffuse_map.wrap_x);
				file.write_uint32(data->meshes[j].draw_ranges[k].diffuse_map.wrap_y);
				write_animation_data(file, data->meshes[j].draw_ranges[k].diffuse_map.uvw_offset);
				write_animation_data(file, data->meshes[j].draw_ranges[k].diffuse_map.uvw_rotation);
				write_animation_data(file, data->meshes[j].draw_ranges[k].diffuse_map.uvw_scale);

				file.write_int32(data->meshes[j].draw_ranges[k].specular_map.texture);
				file.write_int32(data->meshes[j].draw_ranges[k].specular_map.channel);
				file.write_uint32(data->meshes[j].draw_ranges[k].specular_map.wrap_x);
				file.write_uint32(data->meshes[j].draw_ranges[k].specular_map.wrap_y);
				write_animation_data(file, data->meshes[j].draw_ranges[k].specular_map.uvw_offset);
				write_animation_data(file, data->meshes[j].draw_ranges[k].specular_map.uvw_rotation);
				write_animation_data(file, data->meshes[j].draw_ranges[k].specular_map.uvw_scale);

				file.write_int32(data->meshes[j].draw_ranges[k].bumpmap_map.texture);
				file.write_int32(data->meshes[j].draw_ranges[k].bumpmap_map.channel);
				file.write_uint32(data->meshes[j].draw_ranges[k].bumpmap_map.wrap_x);
				file.write_uint32(data->meshes[j].draw_ranges[k].bumpmap_map.wrap_y);
				write_animation_data(file, data->meshes[j].draw_ranges[k].bumpmap_map.uvw_offset);
				write_animation_data(file, data->meshes[j].draw_ranges[k].bumpmap_map.uvw_rotation);
				write_animation_data(file, data->meshes[j].draw_ranges[k].bumpmap_map.uvw_scale);

				file.write_int32(data->meshes[j].draw_ranges[k].self_illumination_map.texture);
				file.write_int32(data->meshes[j].draw_ranges[k].self_illumination_map.channel);
				file.write_uint32(data->meshes[j].draw_ranges[k].self_illumination_map.wrap_x);
				file.write_uint32(data->meshes[j].draw_ranges[k].self_illumination_map.wrap_y);
				write_animation_data(file, data->meshes[j].draw_ranges[k].self_illumination_map.uvw_offset);
				write_animation_data(file, data->meshes[j].draw_ranges[k].self_illumination_map.uvw_rotation);
				write_animation_data(file, data->meshes[j].draw_ranges[k].self_illumination_map.uvw_scale);

				file.write_int32(data->meshes[j].draw_ranges[k].light_map.texture);
				file.write_int32(data->meshes[j].draw_ranges[k].light_map.channel);
				file.write_uint32(data->meshes[j].draw_ranges[k].light_map.wrap_x);
				file.write_uint32(data->meshes[j].draw_ranges[k].light_map.wrap_y);
				write_animation_data(file, data->meshes[j].draw_ranges[k].light_map.uvw_offset);
				write_animation_data(file, data->meshes[j].draw_ranges[k].light_map.uvw_rotation);
				write_animation_data(file, data->meshes[j].draw_ranges[k].light_map.uvw_scale);

				file.write_uint32(data->meshes[j].draw_ranges[k].start_element);
				file.write_uint32(data->meshes[j].draw_ranges[k].num_elements);
			}
		}

		for (size_t i = 0; i < data->textures.size(); i++)
		{
			file.write_float(data->textures[i].gamma);
			file.write_string_a(PathHelp::make_relative(base_path, data->textures[i].name));
		}

		for (size_t i = 0; i < data->bones.size(); i++)
		{
			file.write_uint32(data->bones[i].billboarded ? 1 : 0);
			file.write_int16(data->bones[i].parent_bone);

			write_animation_data(file, data->bones[i].position);
			write_animation_data(file, data->bones[i].orientation);

			file.write_float(data->bones[i].pivot.x);
			file.write_float(data->bones[i].pivot.y);
			file.write_float(data->bones[i].pivot.z);
		}

		for (size_t i = 0; i < data->lights.size(); i++)
		{
			file.write_uint32(data->lights[i].bone_selector);
			file.write_uint8(data->lights[i].casts_shadows ? 1 : 0);
			file.write_uint8(data->lights[i].rectangle ? 1 : 0);

			write_animation_data(file, data->lights[i].position);
			write_animation_data(file, data->lights[i].orientation);
			write_animation_data(file, data->lights[i].color);
			write_animation_data(file, data->lights[i].attenuation_start);
			write_animation_data(file, data->lights[i].attenuation_end);
			write_animation_data(file, data->lights[i].falloff);
			write_animation_data(file, data->lights[i].hotspot);
			write_animation_data(file, data->lights[i].aspect);
		}

		for (size_t i = 0; i < data->cameras.size(); i++)
		{
			write_animation_data(file, data->cameras[i].position);
			write_animation_data(file, data->cameras[i].orientation);

			file.write_float(data->cameras[i].fov_y);
		}

		std::vector<clan::Vec3f> attachments_position;
		std::vector<int> attachments_bone_selector;
		for (size_t i = 0; i < data->attachment_points.size(); i++)
		{
			attachments_position.push_back(data->attachment_points[i].position);
			attachments_bone_selector.push_back(data->attachment_points[i].bone_selector);
		}
		write_vector_contents(file, attachments_position);
		write_vector_contents(file, attachments_bone_selector);

		for (size_t i = 0; i < data->particle_emitters.size(); i++)
		{
			// To do: implement particle emitters
		}

		for (size_t i = 0; i < data->animations.size(); i++)
		{
			file.write_string_a(data->animations[i].name);
			file.write_float(data->animations[i].length);
			file.write_uint32(data->animations[i].loop ? 1 : 0);
			file.write_float(data->animations[i].playback_speed);
			file.write_float(data->animations[i].moving_speed);
			file.write_uint16(data->animations[i].rarity);
		}
	}

	inline std::shared_ptr<clan::ModelData> CModelFormat::load(const std::string &filename)
	{
		clan::File file(filename);
		return load(file, clan::PathHelp::get_fullpath(filename));
	}

	inline std::shared_ptr<clan::ModelData> CModelFormat::load(clan::IODevice &file, const std::string &base_path)
	{
		int version = file.read_uint32();
		char magic[12];
		file.read(magic, 12);
		if (memcmp(magic, "ModelCaramba", 12) != 0)
			throw clan::Exception("Not a Caramba Model file");

		if (version < 6 || version > 13)
			throw clan::Exception("Unsupported file version");

		std::shared_ptr<clan::ModelData> data(new clan::ModelData());

		if (version < 11)
			file.read_uint32();
		data->textures.resize(file.read_uint32());
		if (version < 11)
			file.read_uint32();
		data->bones.resize(file.read_uint32());
		data->lights.resize(file.read_uint32());
		data->cameras.resize(file.read_uint32());
		data->attachment_points.resize(file.read_uint32());
		data->particle_emitters.resize(file.read_uint32());
		data->animations.resize(file.read_uint32());
		data->aabb_min.x = file.read_float();
		data->aabb_min.y = file.read_float();
		data->aabb_min.z = file.read_float();
		data->aabb_max.x = file.read_float();
		data->aabb_max.y = file.read_float();
		data->aabb_max.z = file.read_float();

		data->meshes.resize(file.read_uint32());
		if (version < 11)
		{
			file.read_float();
			file.read_float();
		}
		for (size_t j = 0; j < data->meshes.size(); j++)
		{
			data->meshes[j].vertices.resize(file.read_uint32());
			data->meshes[j].normals.resize(file.read_uint32());
			data->meshes[j].tangents.resize(file.read_uint32());
			data->meshes[j].bitangents.resize(file.read_uint32());
			data->meshes[j].bone_weights.resize(file.read_uint32());
			data->meshes[j].bone_selectors.resize(file.read_uint32());
			data->meshes[j].colors.resize(file.read_uint32());
			data->meshes[j].channels.resize(file.read_uint32());
			data->meshes[j].elements.resize(file.read_uint32());
			data->meshes[j].draw_ranges.resize(file.read_uint32());

			read_vector_contents(file, data->meshes[j].vertices);
			read_vector_contents(file, data->meshes[j].normals);
			read_vector_contents(file, data->meshes[j].tangents);
			read_vector_contents(file, data->meshes[j].bitangents);
			read_vector_contents(file, data->meshes[j].bone_weights);
			read_vector_contents(file, data->meshes[j].bone_selectors);
			read_vector_contents(file, data->meshes[j].colors);

			for (size_t k = 0; k < data->meshes[j].channels.size(); k++)
			{
				data->meshes[j].channels[k].resize(file.read_uint32());
				read_vector_contents(file, data->meshes[j].channels[k]);
			}

			read_vector_contents(file, data->meshes[j].elements);

			for (size_t k = 0; k < data->meshes[j].draw_ranges.size(); k++)
			{
				data->meshes[j].draw_ranges[k].ambient.set_single_value(clan::Vec3f(file.read_float(), file.read_float(), file.read_float()));
				data->meshes[j].draw_ranges[k].diffuse.set_single_value(clan::Vec3f(file.read_float(), file.read_float(), file.read_float()));
				data->meshes[j].draw_ranges[k].specular.set_single_value(clan::Vec3f(file.read_float(), file.read_float(), file.read_float()));

				data->meshes[j].draw_ranges[k].glossiness.set_single_value(file.read_float());
				data->meshes[j].draw_ranges[k].specular_level.set_single_value(file.read_float());

				if (version < 10)
				{
					float self_illumination_amount = 1.0f;
					data->meshes[j].draw_ranges[k].self_illumination_amount.timelines.resize(data->animations.size());
					for (size_t anim_index = 0; anim_index < data->animations.size(); anim_index++)
					{
						data->meshes[j].draw_ranges[k].self_illumination_amount.timelines[anim_index].timestamps.resize(1);
						data->meshes[j].draw_ranges[k].self_illumination_amount.timelines[anim_index].values.push_back(self_illumination_amount);
					}
				}
				else
				{
					read_animation_data(file, data->meshes[j].draw_ranges[k].self_illumination_amount);
				}

				if (version < 8)
				{
					float self_illumination = file.read_float();
					data->meshes[j].draw_ranges[k].self_illumination.timelines.resize(data->animations.size());
					for (size_t anim_index = 0; anim_index < data->animations.size(); anim_index++)
					{
						data->meshes[j].draw_ranges[k].self_illumination.timelines[anim_index].timestamps.resize(1);
						data->meshes[j].draw_ranges[k].self_illumination.timelines[anim_index].values.push_back(clan::Vec3f(self_illumination));
					}
				}
				else if (version == 8)
				{
					float self_illumination_amount = 1.0f;
					clan::ModelDataAnimationData<float> self_illumination_old;
					read_animation_data(file, self_illumination_old);
					data->meshes[j].draw_ranges[k].self_illumination.timelines.resize(self_illumination_old.timelines.size());
					for (size_t anim_index = 0; anim_index < self_illumination_old.timelines.size(); anim_index++)
					{
						data->meshes[j].draw_ranges[k].self_illumination.timelines[anim_index].timestamps = self_illumination_old.timelines[anim_index].timestamps;
						for (size_t time_index = 0; time_index < self_illumination_old.timelines[anim_index].values.size(); time_index++)
						{
							float self_illumination = self_illumination_old.timelines[anim_index].values[time_index];
							data->meshes[j].draw_ranges[k].self_illumination.timelines[anim_index].values.push_back(clan::Vec3f(self_illumination));
						}
					}
				}
				else
				{
					read_animation_data(file, data->meshes[j].draw_ranges[k].self_illumination);
				}

				data->meshes[j].draw_ranges[k].two_sided = (file.read_uint32() == 1);
				if (version > 8)
					data->meshes[j].draw_ranges[k].transparent = (file.read_uint32() == 1);

				if (version < 11)
					file.read_uint32();
				data->meshes[j].draw_ranges[k].diffuse_map.texture = file.read_uint32();
				data->meshes[j].draw_ranges[k].diffuse_map.channel = file.read_uint32();
				data->meshes[j].draw_ranges[k].diffuse_map.wrap_x = (clan::ModelDataTextureMap::WrapMode)file.read_uint32();
				data->meshes[j].draw_ranges[k].diffuse_map.wrap_y = (clan::ModelDataTextureMap::WrapMode)file.read_uint32();
				if (version < 8)
				{
					clan::Mat4f uv_transform;
					file.read(uv_transform.matrix, 16 * 4);

					clan::Vec3f uvw_offset;
					clan::Quaternionf uvw_rotation;
					clan::Vec3f uvw_scale;
					uv_transform.decompose(uvw_offset, uvw_rotation, uvw_scale);

					data->meshes[j].draw_ranges[k].diffuse_map.uvw_offset.timelines.resize(data->animations.size());
					data->meshes[j].draw_ranges[k].diffuse_map.uvw_rotation.timelines.resize(data->animations.size());
					data->meshes[j].draw_ranges[k].diffuse_map.uvw_scale.timelines.resize(data->animations.size());
					for (size_t anim_index = 0; anim_index < data->animations.size(); anim_index++)
					{
						data->meshes[j].draw_ranges[k].diffuse_map.uvw_offset.timelines[anim_index].timestamps.resize(1);
						data->meshes[j].draw_ranges[k].diffuse_map.uvw_rotation.timelines[anim_index].timestamps.resize(1);
						data->meshes[j].draw_ranges[k].diffuse_map.uvw_scale.timelines[anim_index].timestamps.resize(1);

						data->meshes[j].draw_ranges[k].diffuse_map.uvw_offset.timelines[anim_index].values.push_back(uvw_offset);
						data->meshes[j].draw_ranges[k].diffuse_map.uvw_rotation.timelines[anim_index].values.push_back(uvw_rotation);
						data->meshes[j].draw_ranges[k].diffuse_map.uvw_scale.timelines[anim_index].values.push_back(uvw_scale);
					}
				}
				else
				{
					read_animation_data(file, data->meshes[j].draw_ranges[k].diffuse_map.uvw_offset);
					read_animation_data(file, data->meshes[j].draw_ranges[k].diffuse_map.uvw_rotation);
					read_animation_data(file, data->meshes[j].draw_ranges[k].diffuse_map.uvw_scale);
				}

				if (version < 11)
					file.read_uint32();
				data->meshes[j].draw_ranges[k].specular_map.texture = file.read_uint32();
				data->meshes[j].draw_ranges[k].specular_map.channel = file.read_uint32();
				data->meshes[j].draw_ranges[k].specular_map.wrap_x = (clan::ModelDataTextureMap::WrapMode)file.read_uint32();
				data->meshes[j].draw_ranges[k].specular_map.wrap_y = (clan::ModelDataTextureMap::WrapMode)file.read_uint32();
				if (version < 8)
				{
					clan::Mat4f uv_transform;
					file.read(uv_transform.matrix, 16 * 4);

					clan::Vec3f uvw_offset;
					clan::Quaternionf uvw_rotation;
					clan::Vec3f uvw_scale;
					uv_transform.decompose(uvw_offset, uvw_rotation, uvw_scale);

					data->meshes[j].draw_ranges[k].specular_map.uvw_offset.timelines.resize(data->animations.size());
					data->meshes[j].draw_ranges[k].specular_map.uvw_rotation.timelines.resize(data->animations.size());
					data->meshes[j].draw_ranges[k].specular_map.uvw_scale.timelines.resize(data->animations.size());
					for (size_t anim_index = 0; anim_index < data->animations.size(); anim_index++)
					{
						data->meshes[j].draw_ranges[k].specular_map.uvw_offset.timelines[anim_index].timestamps.resize(1);
						data->meshes[j].draw_ranges[k].specular_map.uvw_rotation.timelines[anim_index].timestamps.resize(1);
						data->meshes[j].draw_ranges[k].specular_map.uvw_scale.timelines[anim_index].timestamps.resize(1);

						data->meshes[j].draw_ranges[k].specular_map.uvw_offset.timelines[anim_index].values.push_back(uvw_offset);
						data->meshes[j].draw_ranges[k].specular_map.uvw_rotation.timelines[anim_index].values.push_back(uvw_rotation);
						data->meshes[j].draw_ranges[k].specular_map.uvw_scale.timelines[anim_index].values.push_back(uvw_scale);
					}
				}
				else
				{
					read_animation_data(file, data->meshes[j].draw_ranges[k].specular_map.uvw_offset);
					read_animation_data(file, data->meshes[j].draw_ranges[k].specular_map.uvw_rotation);
					read_animation_data(file, data->meshes[j].draw_ranges[k].specular_map.uvw_scale);
				}

				if (version < 11)
					file.read_uint32();
				data->meshes[j].draw_ranges[k].bumpmap_map.texture = file.read_uint32();
				data->meshes[j].draw_ranges[k].bumpmap_map.channel = file.read_uint32();
				data->meshes[j].draw_ranges[k].bumpmap_map.wrap_x = (clan::ModelDataTextureMap::WrapMode)file.read_uint32();
				data->meshes[j].draw_ranges[k].bumpmap_map.wrap_y = (clan::ModelDataTextureMap::WrapMode)file.read_uint32();
				if (version < 8)
				{
					clan::Mat4f uv_transform;
					file.read(uv_transform.matrix, 16 * 4);

					clan::Vec3f uvw_offset;
					clan::Quaternionf uvw_rotation;
					clan::Vec3f uvw_scale;
					uv_transform.decompose(uvw_offset, uvw_rotation, uvw_scale);

					data->meshes[j].draw_ranges[k].bumpmap_map.uvw_offset.timelines.resize(data->animations.size());
					data->meshes[j].draw_ranges[k].bumpmap_map.uvw_rotation.timelines.resize(data->animations.size());
					data->meshes[j].draw_ranges[k].bumpmap_map.uvw_scale.timelines.resize(data->animations.size());
					for (size_t anim_index = 0; anim_index < data->animations.size(); anim_index++)
					{
						data->meshes[j].draw_ranges[k].bumpmap_map.uvw_offset.timelines[anim_index].timestamps.resize(1);
						data->meshes[j].draw_ranges[k].bumpmap_map.uvw_rotation.timelines[anim_index].timestamps.resize(1);
						data->meshes[j].draw_ranges[k].bumpmap_map.uvw_scale.timelines[anim_index].timestamps.resize(1);

						data->meshes[j].draw_ranges[k].bumpmap_map.uvw_offset.timelines[anim_index].values.push_back(uvw_offset);
						data->meshes[j].draw_ranges[k].bumpmap_map.uvw_rotation.timelines[anim_index].values.push_back(uvw_rotation);
						data->meshes[j].draw_ranges[k].bumpmap_map.uvw_scale.timelines[anim_index].values.push_back(uvw_scale);
					}
				}
				else
				{
					read_animation_data(file, data->meshes[j].draw_ranges[k].bumpmap_map.uvw_offset);
					read_animation_data(file, data->meshes[j].draw_ranges[k].bumpmap_map.uvw_rotation);
					read_animation_data(file, data->meshes[j].draw_ranges[k].bumpmap_map.uvw_scale);
				}

				if (version < 11)
					file.read_uint32();
				data->meshes[j].draw_ranges[k].self_illumination_map.texture = file.read_uint32();
				data->meshes[j].draw_ranges[k].self_illumination_map.channel = file.read_uint32();
				data->meshes[j].draw_ranges[k].self_illumination_map.wrap_x = (clan::ModelDataTextureMap::WrapMode)file.read_uint32();
				data->meshes[j].draw_ranges[k].self_illumination_map.wrap_y = (clan::ModelDataTextureMap::WrapMode)file.read_uint32();
				if (version < 8)
				{
					clan::Mat4f uv_transform;
					file.read(uv_transform.matrix, 16 * 4);

					clan::Vec3f uvw_offset;
					clan::Quaternionf uvw_rotation;
					clan::Vec3f uvw_scale;
					uv_transform.decompose(uvw_offset, uvw_rotation, uvw_scale);

					data->meshes[j].draw_ranges[k].self_illumination_map.uvw_offset.timelines.resize(data->animations.size());
					data->meshes[j].draw_ranges[k].self_illumination_map.uvw_rotation.timelines.resize(data->animations.size());
					data->meshes[j].draw_ranges[k].self_illumination_map.uvw_scale.timelines.resize(data->animations.size());
					for (size_t anim_index = 0; anim_index < data->animations.size(); anim_index++)
					{
						data->meshes[j].draw_ranges[k].self_illumination_map.uvw_offset.timelines[anim_index].timestamps.resize(1);
						data->meshes[j].draw_ranges[k].self_illumination_map.uvw_rotation.timelines[anim_index].timestamps.resize(1);
						data->meshes[j].draw_ranges[k].self_illumination_map.uvw_scale.timelines[anim_index].timestamps.resize(1);

						data->meshes[j].draw_ranges[k].self_illumination_map.uvw_offset.timelines[anim_index].values.push_back(uvw_offset);
						data->meshes[j].draw_ranges[k].self_illumination_map.uvw_rotation.timelines[anim_index].values.push_back(uvw_rotation);
						data->meshes[j].draw_ranges[k].self_illumination_map.uvw_scale.timelines[anim_index].values.push_back(uvw_scale);
					}
				}
				else
				{
					read_animation_data(file, data->meshes[j].draw_ranges[k].self_illumination_map.uvw_offset);
					read_animation_data(file, data->meshes[j].draw_ranges[k].self_illumination_map.uvw_rotation);
					read_animation_data(file, data->meshes[j].draw_ranges[k].self_illumination_map.uvw_scale);
				}

				if (version > 12)
				{
					data->meshes[j].draw_ranges[k].light_map.texture = file.read_uint32();
					data->meshes[j].draw_ranges[k].light_map.channel = file.read_uint32();
					data->meshes[j].draw_ranges[k].light_map.wrap_x = (clan::ModelDataTextureMap::WrapMode)file.read_uint32();
					data->meshes[j].draw_ranges[k].light_map.wrap_y = (clan::ModelDataTextureMap::WrapMode)file.read_uint32();

					read_animation_data(file, data->meshes[j].draw_ranges[k].light_map.uvw_offset);
					read_animation_data(file, data->meshes[j].draw_ranges[k].light_map.uvw_rotation);
					read_animation_data(file, data->meshes[j].draw_ranges[k].light_map.uvw_scale);
				}

				data->meshes[j].draw_ranges[k].start_element = file.read_uint32();
				data->meshes[j].draw_ranges[k].num_elements = file.read_uint32();
			}
		}

		for (size_t i = 0; i < data->textures.size(); i++)
		{
			data->textures[i].gamma = file.read_float();
			data->textures[i].name = clan::PathHelp::combine(base_path, file.read_string_a());
		}

		for (size_t i = 0; i < data->bones.size(); i++)
		{
			data->bones[i].billboarded = file.read_uint32() == 1;
			data->bones[i].parent_bone = file.read_int16();

			read_animation_data(file, data->bones[i].position);
			read_animation_data(file, data->bones[i].orientation);

			if (version < 12)
			{
				ModelDataAnimationData<Vec3f> scale;
				read_animation_data(file, scale);
			}

			data->bones[i].pivot.x = file.read_float();
			data->bones[i].pivot.y = file.read_float();
			data->bones[i].pivot.z = file.read_float();
		}

		if (version < 7)
		{
			std::vector<clan::Vec3f> lights_position(data->lights.size());
			std::vector<clan::Quaternionf> lights_orientation(data->lights.size());
			std::vector<int> lights_bone_selector(data->lights.size());
			std::vector<clan::Vec3f> lights_color(data->lights.size());
			std::vector<float> lights_attenuation_start(data->lights.size());
			std::vector<float> lights_attenuation_end(data->lights.size());
			std::vector<float> lights_falloff(data->lights.size());
			std::vector<float> lights_hotspot(data->lights.size());
			std::vector<unsigned char> lights_casts_shadows(data->lights.size());
			std::vector<unsigned char> lights_rectangle(data->lights.size());
			std::vector<float> lights_aspect(data->lights.size());
			read_vector_contents(file, lights_position);
			read_vector_contents(file, lights_orientation);
			read_vector_contents(file, lights_bone_selector);
			read_vector_contents(file, lights_color);
			read_vector_contents(file, lights_attenuation_start);
			read_vector_contents(file, lights_attenuation_end);
			read_vector_contents(file, lights_falloff);
			read_vector_contents(file, lights_hotspot);
			read_vector_contents(file, lights_casts_shadows);
			read_vector_contents(file, lights_rectangle);
			read_vector_contents(file, lights_aspect);
			for (size_t i = 0; i < data->lights.size(); i++)
			{
				data->lights[i].position.timelines.resize(data->animations.size());
				data->lights[i].orientation.timelines.resize(data->animations.size());
				data->lights[i].color.timelines.resize(data->animations.size());
				data->lights[i].attenuation_start.timelines.resize(data->animations.size());
				data->lights[i].attenuation_end.timelines.resize(data->animations.size());
				data->lights[i].falloff.timelines.resize(data->animations.size());
				data->lights[i].hotspot.timelines.resize(data->animations.size());
				data->lights[i].aspect.timelines.resize(data->animations.size());
				data->lights[i].ambient_illumination.timelines.resize(data->animations.size());
				for (size_t j = 0; j < data->animations.size(); j++)
				{
					data->lights[i].position.timelines[j].timestamps.resize(1);
					data->lights[i].orientation.timelines[j].timestamps.resize(1);
					data->lights[i].color.timelines[j].timestamps.resize(1);
					data->lights[i].attenuation_start.timelines[j].timestamps.resize(1);
					data->lights[i].attenuation_end.timelines[j].timestamps.resize(1);
					data->lights[i].falloff.timelines[j].timestamps.resize(1);
					data->lights[i].hotspot.timelines[j].timestamps.resize(1);
					data->lights[i].aspect.timelines[j].timestamps.resize(1);

					data->lights[i].bone_selector = lights_bone_selector[i];
					data->lights[i].casts_shadows = (lights_casts_shadows[i] == 1);
					data->lights[i].rectangle = (lights_rectangle[i] == 1);

					data->lights[i].position.timelines[j].values.push_back(lights_position[i]);
					data->lights[i].orientation.timelines[j].values.push_back(lights_orientation[i]);
					data->lights[i].color.timelines[j].values.push_back(lights_color[i]);
					data->lights[i].attenuation_start.timelines[j].values.push_back(lights_attenuation_start[i]);
					data->lights[i].attenuation_end.timelines[j].values.push_back(lights_attenuation_end[i]);
					data->lights[i].falloff.timelines[j].values.push_back(lights_falloff[i]);
					data->lights[i].hotspot.timelines[j].values.push_back(lights_hotspot[i]);
					data->lights[i].aspect.timelines[j].values.push_back(lights_aspect[i]);

					data->lights[i].ambient_illumination.timelines[j].values.push_back(0.025f);
				}
			}
		}
		else
		{
			for (size_t i = 0; i < data->lights.size(); i++)
			{
				data->lights[i].bone_selector = file.read_uint32();
				data->lights[i].casts_shadows = file.read_uint8() != 0;
				data->lights[i].rectangle = file.read_uint8() != 0;

				read_animation_data(file, data->lights[i].position);
				read_animation_data(file, data->lights[i].orientation);
				read_animation_data(file, data->lights[i].color);
				read_animation_data(file, data->lights[i].attenuation_start);
				read_animation_data(file, data->lights[i].attenuation_end);
				read_animation_data(file, data->lights[i].falloff);
				read_animation_data(file, data->lights[i].hotspot);
				read_animation_data(file, data->lights[i].aspect);

				data->lights[i].ambient_illumination.timelines.resize(data->animations.size());
				for (size_t j = 0; j < data->animations.size(); j++)
					data->lights[i].ambient_illumination.timelines[j].values.push_back(0.025f);
			}
		}

		for (size_t i = 0; i < data->cameras.size(); i++)
		{
			read_animation_data(file, data->cameras[i].position);
			read_animation_data(file, data->cameras[i].orientation);

			data->cameras[i].fov_y = file.read_float();
		}

		std::vector<clan::Vec3f> attachments_position(data->attachment_points.size());
		std::vector<int> attachments_bone_selector(data->attachment_points.size());
		read_vector_contents(file, attachments_position);
		read_vector_contents(file, attachments_bone_selector);
		for (size_t i = 0; i < data->attachment_points.size(); i++)
		{
			data->attachment_points[i].position = attachments_position[i];
			data->attachment_points[i].bone_selector = attachments_bone_selector[i];
		}

		for (size_t i = 0; i < data->particle_emitters.size(); i++)
		{
			// To do: implement particle emitters
		}

		for (size_t i = 0; i < data->animations.size(); i++)
		{
			data->animations[i].name = file.read_string_a();
			data->animations[i].length = file.read_float();
			data->animations[i].loop = (file.read_uint32() == 1);
			data->animations[i].playback_speed = file.read_float();
			data->animations[i].moving_speed = file.read_float();
			data->animations[i].rarity = file.read_uint16();
		}

		return data;
	}

	template<typename Type>
	void CModelFormat::write_animation_data(clan::IODevice &file, const clan::ModelDataAnimationData<Type> &animation_data)
	{
		file.write_uint32(animation_data.timelines.size());
		for (size_t j = 0; j < animation_data.timelines.size(); j++)
		{
			file.write_uint32(animation_data.timelines[j].timestamps.size());
			file.write_uint32(animation_data.timelines[j].values.size());
			write_vector_contents(file, animation_data.timelines[j].timestamps);
			write_vector_contents(file, animation_data.timelines[j].values);
		}
	}

	template<typename Type>
	void CModelFormat::write_vector_contents(clan::IODevice &file, const std::vector<Type> &vector_data)
	{
		if (!vector_data.empty())
			file.write(&vector_data[0], vector_data.size() * sizeof(Type));
	}

	template<typename Type>
	inline void CModelFormat::read_animation_data(clan::IODevice &file, clan::ModelDataAnimationData<Type> &out_animation_data)
	{
		out_animation_data.timelines.resize(file.read_uint32());
		for (size_t j = 0; j < out_animation_data.timelines.size(); j++)
		{
			clan::ModelDataAnimationTimeline<Type> data;

			data.timestamps.resize(file.read_uint32());
			data.values.resize(file.read_uint32());
			read_vector_contents(file, data.timestamps);
			read_vector_contents(file, data.values);

			if (data.timestamps.size() != data.values.size())
				throw clan::Exception("Corrupt model file");

			// Remove redundant data:
			out_animation_data.timelines[j].timestamps.reserve(data.timestamps.size());
			out_animation_data.timelines[j].values.reserve(data.values.size());
			for (size_t i = 0; i < data.timestamps.size(); i++)
			{
				if (i == 0 || out_animation_data.timelines[j].values.back() != data.values[i])
				{
					out_animation_data.timelines[j].timestamps.push_back(data.timestamps[i]);
					out_animation_data.timelines[j].values.push_back(data.values[i]);
				}
			}
		}
	}

	template<typename Type>
	void CModelFormat::read_vector_contents(clan::IODevice &file, std::vector<Type> &out_vector_data)
	{
		if (!out_vector_data.empty())
			file.read(&out_vector_data[0], out_vector_data.size() * sizeof(Type));
	}
}
