
#include "precomp.h"
#include "lightmap_texture.h"
#include <array>
#include <algorithm>

namespace clan
{
	void LightmapTexture::generate(const std::shared_ptr<ModelData> &new_model_data)
	{
		model_data = new_model_data;
		model_collision = Physics3DObject();
		world = Physics3DWorld();
		model_collision = Physics3DObject::collision_body(world, Physics3DShape::model(model_data));

		for (auto &mesh : model_data->meshes)
		{
			triangle_mesh = std::make_shared<TriangleMeshShape>(mesh.vertices.data(), (int)mesh.vertices.size(), mesh.elements.data(), (int)mesh.elements.size());

			for (auto &range : mesh.draw_ranges)
			{
				if (range.self_illumination_map.channel == -1 || range.self_illumination_map.texture == -1)
					continue;

				for (size_t element_index = range.start_element; element_index + 2 < range.start_element + range.num_elements; element_index += 3)
				{
					unsigned int indexes[3] =
					{
						mesh.elements[element_index],
						mesh.elements[element_index + 1],
						mesh.elements[element_index + 2]
					};

					Vec2f uv[3] =
					{
						mesh.channels[range.self_illumination_map.channel][indexes[0]] * 1024.0f,
						mesh.channels[range.self_illumination_map.channel][indexes[1]] * 1024.0f,
						mesh.channels[range.self_illumination_map.channel][indexes[2]] * 1024.0f
					};

					Vec3f vertices[3] =
					{
						mesh.vertices[indexes[0]],
						mesh.vertices[indexes[1]],
						mesh.vertices[indexes[2]]
					};

					Vec3f normals[3] =
					{
						mesh.normals[indexes[0]],
						mesh.normals[indexes[1]],
						mesh.normals[indexes[2]]
					};

					raytrace_face(range.self_illumination_map.texture, uv, vertices, normals);
				}
			}
		}

		for (auto &it : lightmaps)
		{
			auto &texture = model_data->textures[it.first];
			auto &buffer = it.second;

			auto pixelbuffer = PixelBuffer(buffer->width(), buffer->height(), tf_rgb32f, buffer->data(), true).to_format(tf_rgba8);
			PNGProvider::save(pixelbuffer, texture.name);
		}
	}

	void LightmapTexture::raytrace_face(int target_texture, const Vec2f *points, const Vec3f *vertices, const Vec3f *normals)
	{
		auto &lightmap = lightmaps[target_texture];
		if (!lightmap)
			lightmap = std::make_shared<LightmapBuffer>(1024, 1024); // To do: it needs to read this from the mesh somehow

		Vec2f sorted_points[3] = { points[0], points[1], points[2] };
		if (sorted_points[0].y > sorted_points[1].y) std::swap(sorted_points[0], sorted_points[1]);
		if (sorted_points[0].y > sorted_points[2].y) std::swap(sorted_points[0], sorted_points[2]);
		if (sorted_points[1].y > sorted_points[2].y) std::swap(sorted_points[1], sorted_points[2]);

		Vec2f dir[3] =
		{
			sorted_points[1] - sorted_points[0],
			sorted_points[2] - sorted_points[1],
			sorted_points[2] - sorted_points[0]
		};

		int start_y = (int)std::round(sorted_points[0].y);
		int middle_y = (int)std::round(sorted_points[1].y);
		int end_y = (int)std::round(sorted_points[2].y);

		start_y = std::max(start_y, 0);
		middle_y = std::max(std::min(middle_y, lightmap->height()), start_y);
		end_y = std::min(end_y, lightmap->height());

		for (int y = start_y; y < middle_y; y++)
		{
			int x0 = (int)std::round(sorted_points[0].x + (y + 0.5f - sorted_points[0].y) * dir[0].x / dir[0].y);
			int x1 = (int)std::round(sorted_points[0].x + (y + 0.5f - sorted_points[0].y) * dir[2].x / dir[2].y);
			if (x0 > x1) std::swap(x0, x1);
			x0 = std::max(x0, 0);
			x1 = std::min(x1, lightmap->width());
			for (int x = x0; x < x1; x++)
				lightmap->at(x, y) = raytrace_face_point(target_texture, points, x + 0.5f, y + 0.5f, vertices, normals);
		}

		for (int y = middle_y; y < end_y; y++)
		{
			int x0 = (int)std::round(sorted_points[1].x + (y + 0.5f - sorted_points[1].y) * dir[1].x / dir[1].y);
			int x1 = (int)std::round(sorted_points[0].x + (y + 0.5f - sorted_points[0].y) * dir[2].x / dir[2].y);
			if (x0 > x1) std::swap(x0, x1);
			x0 = std::max(x0, 0);
			x1 = std::min(x1, lightmap->width());
			for (int x = x0; x < x1; x++)
				lightmap->at(x, y) = raytrace_face_point(target_texture, points, x + 0.5f, y + 0.5f, vertices, normals);
		}
	}

	Vec3f LightmapTexture::raytrace_face_point(int target_texture, const Vec2f *uv, float px, float py, const Vec3f *vertices, const Vec3f *normals)
	{
		// Find barycentric coordinates for our position:

		float det = ((uv[1].y - uv[2].y) * (uv[0].x - uv[2].x) + (uv[2].x - uv[1].x) * (uv[0].y - uv[2].y));
		if (det == 0.0f || det == -0.0f)
			return Vec3f();
		float a = ((uv[1].y - uv[2].y) * (px - uv[2].x) + (uv[2].x - uv[1].x) * (py - uv[2].y)) / det;
		float b = ((uv[2].y - uv[0].y) * (px - uv[2].x) + (uv[0].x - uv[2].x) * (py - uv[2].y)) / det;
		float c = 1.0f - a - b;

		// To do: clamp position to stay within our face
		// http://math.stackexchange.com/questions/1092912/find-closest-point-in-triangle-given-barycentric-coordinates-outside

		Vec3f fragment_pos = a * vertices[0] + b * vertices[1] + c * vertices[2];

		// To do: use vertex normals instead of face normal
		//Vec3f fragment_normal = Vec3f::normalize(Vec3f::cross(vertices[1] - vertices[0], vertices[2] - vertices[0]));
		Vec3f fragment_normal = Vec3f::normalize(a * normals[0] + b * normals[1] + c * normals[2]);

		Vec3f diffuse_contribution;

		//Physics3DRayTest ray_test(world);
		for (const auto &light : model_data->lights)
		{
			Vec3f light_pos = light.position.get_single_value();
			Vec3f fragment_to_light = light_pos - fragment_pos;
			float distance_to_light = fragment_to_light.length();

			if (distance_to_light > light.attenuation_end.get_single_value())
				continue;

			float margin = 0.01f;
			//if (!ray_test.test(fragment_pos + fragment_normal * margin, light_pos))
			if (!TriangleMeshShape::find_any_hit(triangle_mesh.get(), fragment_pos + fragment_normal * margin, light_pos))
			{
				Vec3f light_direction = (distance_to_light > 0.0f) ? fragment_to_light * (1.0f / distance_to_light) : Vec3f();

				// To do: support more than just point lights

				float attenuation_start = light.attenuation_start.get_single_value();
				float rcp_attenuation_delta = 1.0f / (light.attenuation_end.get_single_value() - light.attenuation_start.get_single_value());
				float distance_attenuation = clamp(1.0f - (distance_to_light - attenuation_start) * rcp_attenuation_delta, 0.0f, 1.0f);

				float lambertian_diffuse_contribution = std::max(Vec3f::dot(fragment_normal, light_direction), 0.0f);
				diffuse_contribution += (distance_attenuation * lambertian_diffuse_contribution) * light.color.get_single_value();
			}
		}

		return diffuse_contribution * 0.25f;
	}
}
