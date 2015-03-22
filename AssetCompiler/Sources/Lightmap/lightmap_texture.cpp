
#include "precomp.h"
#include "lightmap_texture.h"
#include <array>
#include <algorithm>

namespace clan
{
	void LightmapTexture::generate(const std::shared_ptr<ModelData> &new_model_data)
	{
		model_data = new_model_data;

		create_light_maps();
		create_collision_mesh();
		raytrace();
		outline_extend();
		//blur();
		save_lightmaps();
	}

	void LightmapTexture::raytrace()
	{
		// shooting_rays();

		for (auto it : lightmaps)
		{
			int target_texture = it.first;
			auto &lightmap = it.second;

			std::vector<std::future<void>> futures;
			int num_cores = std::max((int)std::thread::hardware_concurrency(), 8);
			for (int core = 0; core < num_cores; core++)
			{
				futures.push_back(std::async(std::launch::async, [&](int start_y)
				{
					for (int y = start_y; y < lightmap->light.height(); y += num_cores)
					{
						for (int x = 0; x < lightmap->light.width(); x++)
						{
							ambient_occulusion(lightmap, x, y);
							//raytrace_visible_lights(lightmap, x, y);
						}
					}
				}, core));
			}
			for (auto &f : futures) f.get();
		}
	}

	void LightmapTexture::ambient_occulusion(std::shared_ptr<LightmapBuffers> &lightmap, int x, int y)
	{
		Vec3f fragment_pos = lightmap->position.at(x, y);
		Vec3f fragment_normal = lightmap->normal.at(x, y);

		if (fragment_normal == Vec3f())
			return;

		Vec3f side = Vec3f::cross(fragment_normal, Vec3f(1.0f, 0.0f, 0.0f));
		if (Vec3f::dot(side, side) < FLT_EPSILON)
			side = Vec3f::cross(fragment_normal, Vec3f(0.0f, 1.0f, 0.0f));
		side.normalize();

		Vec3f up = Vec3f::cross(fragment_normal, side);

		Vec3f ambient_color(0.5f, 0.5f, 0.5f);

		Vec3f ambient_contribution;

		int sqr_samples = 4;// 16;
		int num_samples = sqr_samples * sqr_samples;
		for (int i = 0; i < num_samples; i++)
		{
			int a = i / sqr_samples;
			int b = i % sqr_samples;

			float random1 = rand() / (float)(RAND_MAX + 1);
			float random2 = rand() / (float)(RAND_MAX + 1);
			Vec3f hemisphere_ray = cosine_sample_hemisphere((a + random1) / sqr_samples, (b + random2) / sqr_samples);

			Vec3f dir = side * hemisphere_ray.x + up * hemisphere_ray.y + fragment_normal * hemisphere_ray.z;

			if (!TriangleMeshShape::find_any_hit(triangle_mesh.get(), fragment_pos + fragment_normal * margin, fragment_pos + dir * 0.5f))
			{
				ambient_contribution += ambient_color;
			}
		}

		lightmap->light.at(x, y) = ambient_contribution / (float)num_samples * 0.25f;
	}

	Vec3f LightmapTexture::uniform_sample_hemisphere(float random1, float random2)
	{
		float r = std::sqrt(1.0f - random1 * random1);
		float phi = 2 * PI * random2;
		return Vec3f(std::cos(phi) * r, std::sin(phi) * r, random1);
	}

	Vec3f LightmapTexture::cosine_sample_hemisphere(float random1, float random2)
	{
		float r = std::sqrt(random1);
		float theta = 2.0f * PI * random2;
		float x = r * std::cos(theta);
		float y = r * std::sin(theta);
		return Vec3f(x, y, std::sqrt(std::max(0.0f, 1.0f - random1)));
	}

	void LightmapTexture::raytrace_visible_lights(std::shared_ptr<LightmapBuffers> &lightmap, int x, int y)
	{
		Vec3f fragment_pos = lightmap->position.at(x, y);
		Vec3f fragment_normal = lightmap->normal.at(x, y);

		if (fragment_normal == Vec3f())
			return;

		Vec3f diffuse_contribution;

		//Physics3DRayTest ray_test(world);
		for (const auto &light : model_data->lights)
		{
			Vec3f light_pos = light.position.get_single_value();
			Vec3f fragment_to_light = light_pos - fragment_pos;
			float distance_to_light = fragment_to_light.length();

			if (distance_to_light > light.attenuation_end.get_single_value())
				continue;

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

		lightmap->light.at(x, y) = diffuse_contribution * 0.25f;
	}

	void LightmapTexture::shooting_rays()
	{
		std::vector<ModelDataLight> new_lights;

		for (auto &light : model_data->lights)
		{
			new_lights.push_back(light);

			Vec3f pos = light.position.get_single_value();
			float attenuation_end = light.attenuation_end.get_single_value();

			for (int i = 0; i < 10; i++)
			{
				Vec3f dir;
				while (dir == Vec3f())
				{
					float x = rand() / (float)RAND_MAX * 2.0f - 1.0f;
					float y = rand() / (float)RAND_MAX * 2.0f - 1.0f;
					float z = rand() / (float)RAND_MAX * 2.0f - 1.0f;
					dir = Vec3f(x, y, z);
				}
				dir.normalize();

				Physics3DRayTest ray_test(world);
				if (ray_test.test(pos, pos + dir * attenuation_end))
				{
					float length_travelled = ray_test.get_hit_fraction() * attenuation_end;

					Vec3f new_pos = ray_test.get_hit_position() + ray_test.get_hit_normal() * margin;
					float lambertian_diffuse_contribution = std::max(Vec3f::dot(ray_test.get_hit_normal(), -dir), 0.0f);

					float radian_emittance = 0.5f;

					ModelDataLight new_light = light;
					new_light.position.set_single_value(new_pos);
					new_light.color.set_single_value(light.color.get_single_value() * lambertian_diffuse_contribution * radian_emittance);
					new_light.attenuation_start.set_single_value(std::max(light.attenuation_start.get_single_value() - length_travelled, 0.0f));
					new_light.attenuation_end.set_single_value(std::max(light.attenuation_end.get_single_value() - length_travelled, 0.0f));
					new_lights.push_back(new_light);
				}
			}
		}

		model_data->lights = new_lights;
	}

	void LightmapTexture::outline_extend()
	{
		for (auto it : lightmaps)
		{
			int target_texture = it.first;
			auto &lightmap = it.second;

			for (int y = 0; y < lightmap->light.height(); y++)
			{
				for (int x = 0; x < lightmap->light.width(); x++)
				{
					if (lightmap->normal.at(x, y) == Vec3f())
					{
						Vec3f light;
						int count = 0;
						for (int dy = -1; dy < 2; dy++)
						{
							for (int dx = -1; dx < 2; dx++)
							{
								if (x + dx > 0 && x + dx < lightmap->light.width() &&
									y + dy > 0 && y + dy < lightmap->light.height() &&
									lightmap->normal.at(x + dx, y + dy) != Vec3f())
								{
									light += lightmap->light.at(x + dx, y + dy);
									count++;
								}
							}
						}
						if (count > 0)
							lightmap->light.at(x, y) = light / (float)count;
					}
				}
			}
		}
	}

	void LightmapTexture::blur()
	{
		for (auto it : lightmaps)
		{
			int target_texture = it.first;
			auto &lightmap = it.second;

			for (int y = 0; y < lightmap->light.height(); y++)
			{
				for (int x = 0; x < lightmap->light.width(); x++)
				{
					if (lightmap->normal.at(x, y) != Vec3f())
					{
						Vec3f light;
						int count = 0;
						for (int dy = -1; dy < 2; dy++)
						{
							for (int dx = -1; dx < 2; dx++)
							{
								if (x + dx > 0 && x + dx < lightmap->light.width() &&
									y + dy > 0 && y + dy < lightmap->light.height() &&
									lightmap->normal.at(x + dx, y + dy) != Vec3f())
								{
									light += lightmap->light.at(x + dx, y + dy);
									count++;
								}
							}
						}
						if (count > 0)
							lightmap->light.at(x, y) = light / (float)count;
					}
				}
			}
		}
	}

	void LightmapTexture::create_collision_mesh()
	{
		model_collision = Physics3DObject();
		world = Physics3DWorld();
		model_collision = Physics3DObject::collision_body(world, Physics3DShape::model(model_data));

		if (model_data->meshes.empty())
			return;

		auto &mesh = model_data->meshes.front();

		triangle_mesh = std::make_shared<TriangleMeshShape>(mesh.vertices.data(), (int)mesh.vertices.size(), mesh.elements.data(), (int)mesh.elements.size());

		for (auto &range : mesh.draw_ranges)
		{
			if (range.transparent || range.alpha_test)
				continue;

			triangle_elements.insert(triangle_elements.end(), mesh.elements.begin() + range.start_element, mesh.elements.begin() + (range.start_element + range.num_elements));
		}
	}

	void LightmapTexture::create_light_maps()
	{
		for (auto &mesh : model_data->meshes)
		{
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

					generate_face(range.self_illumination_map.texture, uv, vertices, normals);
				}
			}
		}
	}

	void LightmapTexture::generate_face(int target_texture, const Vec2f *points, const Vec3f *vertices, const Vec3f *normals)
	{
		auto &lightmap = lightmaps[target_texture];
		if (!lightmap)
			lightmap = std::make_shared<LightmapBuffers>(1024, 1024); // To do: it needs to read this from the mesh somehow

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
		middle_y = std::max(std::min(middle_y, lightmap->light.height()), start_y);
		end_y = std::min(end_y, lightmap->light.height());

		for (int y = start_y; y < middle_y; y++)
		{
			int x0 = (int)std::round(sorted_points[0].x + (y + 0.5f - sorted_points[0].y) * dir[0].x / dir[0].y);
			int x1 = (int)std::round(sorted_points[0].x + (y + 0.5f - sorted_points[0].y) * dir[2].x / dir[2].y);
			if (x0 > x1) std::swap(x0, x1);
			x0 = std::max(x0, 0);
			x1 = std::min(x1, lightmap->light.width());
			for (int x = x0; x < x1; x++)
				generate_face_fragment(lightmap, x, y, target_texture, points, x + 0.5f, y + 0.5f, vertices, normals);
		}

		for (int y = middle_y; y < end_y; y++)
		{
			int x0 = (int)std::round(sorted_points[1].x + (y + 0.5f - sorted_points[1].y) * dir[1].x / dir[1].y);
			int x1 = (int)std::round(sorted_points[0].x + (y + 0.5f - sorted_points[0].y) * dir[2].x / dir[2].y);
			if (x0 > x1) std::swap(x0, x1);
			x0 = std::max(x0, 0);
			x1 = std::min(x1, lightmap->light.width());
			for (int x = x0; x < x1; x++)
				generate_face_fragment(lightmap, x, y, target_texture, points, x + 0.5f, y + 0.5f, vertices, normals);
		}
	}

	void LightmapTexture::generate_face_fragment(std::shared_ptr<LightmapBuffers> &lightmap, int x, int y, int target_texture, const Vec2f *uv, float px, float py, const Vec3f *vertices, const Vec3f *normals)
	{
		// Find barycentric coordinates for our position:

		float det = ((uv[1].y - uv[2].y) * (uv[0].x - uv[2].x) + (uv[2].x - uv[1].x) * (uv[0].y - uv[2].y));
		if (det == 0.0f || det == -0.0f)
			return;
		float a = ((uv[1].y - uv[2].y) * (px - uv[2].x) + (uv[2].x - uv[1].x) * (py - uv[2].y)) / det;
		float b = ((uv[2].y - uv[0].y) * (px - uv[2].x) + (uv[0].x - uv[2].x) * (py - uv[2].y)) / det;
		float c = 1.0f - a - b;

		// To do: clamp position to stay within our face
		// http://math.stackexchange.com/questions/1092912/find-closest-point-in-triangle-given-barycentric-coordinates-outside

		Vec3f fragment_pos = a * vertices[0] + b * vertices[1] + c * vertices[2];
		Vec3f fragment_normal = Vec3f::normalize(a * normals[0] + b * normals[1] + c * normals[2]);

		lightmap->position.at(x, y) = fragment_pos;
		lightmap->normal.at(x, y) = fragment_normal;
	}

	void LightmapTexture::save_lightmaps()
	{
		for (auto &it : lightmaps)
		{
			auto &texture = model_data->textures[it.first];
			auto &buffer = it.second;

			auto pixelbuffer = PixelBuffer(buffer->light.width(), buffer->light.height(), tf_rgb32f, buffer->light.data(), true).to_format(tf_rgba8);
			PNGProvider::save(pixelbuffer, texture.name);
		}
	}
}
