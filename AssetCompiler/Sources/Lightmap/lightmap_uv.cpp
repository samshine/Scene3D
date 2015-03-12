
#include "precomp.h"
#include "lightmap_uv.h"
#include "texture_atlas.h"
#include <algorithm>

namespace clan
{
	LightmapUV::LightmapUV()
	{
	}

	void LightmapUV::generate(const std::shared_ptr<ModelData> &model_data)
	{
		float density = 1.0f;

		TextureAtlas atlas(Size(1024, 1024));

		for (auto &mesh : model_data->meshes)
		{
			mesh.channels.resize(std::max(mesh.channels.size(), (size_t)4));
			auto &lightmap_channel = mesh.channels[3];

			std::vector<bool> vertex_uv_used;
			vertex_uv_used.resize(mesh.vertices.size());

			size_t num_faces = mesh.elements.size() / 3;

			std::vector<int> face_lightmap_texture_index;
			face_lightmap_texture_index.resize(num_faces);

			for (size_t face_index = 0; face_index < num_faces; face_index++)
			{
				unsigned int indexes[3] =
				{
					mesh.elements[face_index * 3],
					mesh.elements[face_index * 3 + 1],
					mesh.elements[face_index * 3 + 2]
				};

				Vec3f *vertices[3] =
				{
					&mesh.vertices[indexes[0]],
					&mesh.vertices[indexes[1]],
					&mesh.vertices[indexes[2]]
				};

				Vec3f face_normal = (*vertices[2] - *vertices[0]).cross(*vertices[1] - *vertices[0]);
				face_normal.x = std::abs(face_normal.x);
				face_normal.y = std::abs(face_normal.y);
				face_normal.z = std::abs(face_normal.z);

				Vec2f uv[3];

				if (face_normal.x >= face_normal.y && face_normal.x >= face_normal.z)
				{
					for (int i = 0; i < 3; i++)
						uv[i] = Vec2f(vertices[i]->y, -vertices[i]->z);
				}
				else if (face_normal.y >= face_normal.x && face_normal.y >= face_normal.z)
				{
					for (int i = 0; i < 3; i++)
						uv[i] = Vec2f(vertices[i]->x, -vertices[i]->z);
				}
				else
				{
					for (int i = 0; i < 3; i++)
						uv[i] = Vec2f(vertices[i]->x, -vertices[i]->y);
				}

				for (int i = 0; i < 3; i++)
				{
					uv[i].x = std::floor(uv[i].x * density);
					uv[i].y = std::floor(uv[i].y * density);
				}

				Vec2f min_uv = Vec2f(std::min(std::min(uv[0].x, uv[1].x), uv[2].x), std::min(std::min(uv[0].y, uv[1].y), uv[2].y));
				for (int i = 0; i < 3; i++)
					uv[i] -= min_uv;

				Vec2f max_uv = Vec2f(std::max(std::max(uv[0].x, uv[1].x), uv[2].x), std::max(std::max(uv[0].y, uv[1].y), uv[2].y));

				TextureAtlasObject atlas_pos = atlas.add(Size((int)max_uv.x, (int)max_uv.y));

				Vec2f offset((float)atlas_pos.box.left, (float)atlas_pos.box.top);
				for (int i = 0; i < 3; i++)
					uv[i] += offset;

				face_lightmap_texture_index[face_index] = atlas_pos.array_index;

				for (int i = 0; i < 3; i++)
				{
					if (vertex_uv_used[indexes[i]])
					{
						unsigned int src_index = indexes[i];
						unsigned int new_index = (unsigned int)mesh.vertices.size();

						if (!mesh.vertices.empty()) { auto value = mesh.vertices[src_index]; mesh.vertices.push_back(value); }
						if (!mesh.tangents.empty()) { auto value = mesh.tangents[src_index]; mesh.tangents.push_back(value); }
						if (!mesh.bitangents.empty()) { auto value = mesh.bitangents[src_index]; mesh.bitangents.push_back(value); }
						if (!mesh.bone_weights.empty()) { auto value = mesh.bone_weights[src_index]; mesh.bone_weights.push_back(value); }
						if (!mesh.bone_selectors.empty()) { auto value = mesh.bone_selectors[src_index]; mesh.bone_selectors.push_back(value); }
						if (!mesh.colors.empty()) { auto value = mesh.colors[src_index]; mesh.colors.push_back(value); }
						for (auto &channel : mesh.channels)
							if (!channel.empty()) { auto value = channel[src_index]; channel.push_back(value); }

						lightmap_channel[new_index] = uv[i];

						mesh.elements[face_index * 3 + i] = new_index;
					}
					else
					{
						vertex_uv_used[indexes[i]] = true;
						lightmap_channel[indexes[i]] = uv[i];
					}
				}
			}

			std::vector<unsigned int> new_elements;
			std::vector<ModelDataDrawRange> new_draw_ranges;

			new_elements.reserve(mesh.elements.size());
			new_draw_ranges.reserve(mesh.draw_ranges.size());

			for (auto &range : mesh.draw_ranges)
			{
				std::map<int, std::vector<unsigned int>> grouped_elements;

				for (int i = 0; i < range.num_elements; i++)
				{
					auto element = mesh.elements[range.start_element + i];
					int array_index = face_lightmap_texture_index[element / 3];
					grouped_elements[array_index].push_back(element);
				}

				int start_offset = 0;

				for (auto it : grouped_elements)
				{
					ModelDataDrawRange new_range = range;
					new_range.start_element += start_offset;
					new_range.num_elements = (int)it.second.size();

					start_offset += new_range.num_elements;

					new_range.self_illumination_map.texture = (int)model_data->textures.size() + it.first;
					new_range.self_illumination_map.channel = 3;

					new_elements.insert(new_elements.end(), it.second.begin(), it.second.end());
					new_draw_ranges.push_back(new_range);
				}
			}

			mesh.elements = new_elements;
			mesh.draw_ranges = new_draw_ranges;
		}

		for (int i = 0; i < atlas.array_count(); i++)
		{
			ModelDataTexture texture;
			texture.gamma = 1.0f;
			texture.name = string_format("lightmap-%1.png", i);
			model_data->textures.push_back(texture);
		}
	}
}