
#include "precomp.h"
#include "lightmap_texture.h"
#include <array>
#include <algorithm>

using namespace uicore;

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

	for (auto &it : lightmaps)
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
						raytrace_visible_lights(lightmap, x, y);
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

	Vec3f ambient_color(0.02f, 0.02f, 0.022f);

	Vec3f ambient_contribution;

	int sqr_samples = 16;
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

	lightmap->light.at(x, y) += ambient_contribution / (float)num_samples;
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

	//auto ray_test = Physics3DRayTest::create(world);
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

	lightmap->light.at(x, y) += diffuse_contribution;
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

			auto ray_test = Physics3DRayTest::create(world);
			if (ray_test->test(pos, pos + dir * attenuation_end))
			{
				float length_travelled = ray_test->hit_fraction() * attenuation_end;

				Vec3f new_pos = ray_test->hit_position() + ray_test->hit_normal() * margin;
				float lambertian_diffuse_contribution = std::max(Vec3f::dot(ray_test->hit_normal(), -dir), 0.0f);

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
	for (auto &it : lightmaps)
	{
		int target_texture = it.first;
		auto &lightmap = it.second;

		for (int i = 0; i < 2; i++)
		{
			for (int y = 0; y < lightmap->light.height(); y++)
			{
				for (int x = 0; x < lightmap->light.width(); x++)
				{
					if (lightmap->light.at(x, y) == Vec3f())
					{
						Vec3f light;
						int count = 0;
						for (int dy = -1; dy < 2; dy++)
						{
							for (int dx = -1; dx < 2; dx++)
							{
								if (x + dx >= 0 && x + dx < lightmap->light.width() &&
									y + dy >= 0 && y + dy < lightmap->light.height() &&
									lightmap->light.at(x + dx, y + dy) != Vec3f())
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
}

void LightmapTexture::blur()
{
	for (auto &it : lightmaps)
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
	world = Physics3DWorld::create();
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
			if (range.light_map.channel == -1 || range.light_map.texture == -1)
				continue;

			for (int element_index = range.start_element; element_index + 2 < range.start_element + range.num_elements; element_index += 3)
			{
				unsigned int indexes[3] =
				{
					mesh.elements[element_index],
					mesh.elements[element_index + 1],
					mesh.elements[element_index + 2]
				};

				Vec2f uv[3] =
				{
					mesh.channels[range.light_map.channel][indexes[0]] * 1024.0f,
					mesh.channels[range.light_map.channel][indexes[1]] * 1024.0f,
					mesh.channels[range.light_map.channel][indexes[2]] * 1024.0f
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

				generate_face(range.light_map.texture, uv, vertices, normals);
			}
		}
	}
}

void LightmapTexture::generate_face(int target_texture, const Vec2f *points, const Vec3f *vertices, const Vec3f *normals)
{
	auto &lightmap = lightmaps[target_texture];
	if (!lightmap)
		lightmap = std::make_shared<LightmapBuffers>(1024, 1024); // To do: it needs to read this from the mesh somehow

	// Triangle rasterization using edge functions
	// See https://fgiesen.wordpress.com/2013/02/08/triangle-rasterization-in-practice/ for details

	// wsum is 2x the signed summed area of the triangle
	float wsum = EdgeFunction::orient_2d(points[0], points[1], points[2]);

	// Triangle is clockwise if the sum is negative
	float sign = (wsum >= 0.0f) ? 1.0f : -1.0f;
	wsum *= sign;

	float rcp_wsum = 1.0f / wsum;

	// Find bounding box
	int min_x = (int)std::floor(std::min({ points[0].x, points[1].x, points[2].x }));
	int min_y = (int)std::floor(std::min({ points[0].y, points[1].y, points[2].y }));
	int max_x = (int)std::floor(std::max({ points[0].x, points[1].x, points[2].x })) + 1;
	int max_y = (int)std::floor(std::max({ points[0].y, points[1].y, points[2].y })) + 1;

	// Clip
	min_x = std::max(min_x, 0);
	min_y = std::max(min_y, 0);
	max_x = std::min(max_x, 1024);
	max_y = std::min(max_y, 1024);

	// Edge functions
	Vec2f origin(min_x + 0.5f, min_y + 0.5f);
	EdgeFunction edge12(points[1], points[2], origin, sign);
	EdgeFunction edge20(points[2], points[0], origin, sign);
	EdgeFunction edge01(points[0], points[1], origin, sign);

	Vec2f origin_tl((float)min_x, (float)min_y);
	EdgeFunction edge12_tl(points[1], points[2], origin_tl, sign);
	EdgeFunction edge20_tl(points[2], points[0], origin_tl, sign);
	EdgeFunction edge01_tl(points[0], points[1], origin_tl, sign);

	// Rasterize
	for (int y = min_y; y < max_y; y++)
	{
		for (int x = min_x; x < max_x; x++)
		{
			// Check if the pixel bounding box includes the triangle
			bool edge12_inside =
				(edge12_tl.value >= 0.0f) ||
				(edge12_tl.value + edge12_tl.step_x >= 0.0f) ||
				(edge12_tl.value + edge12_tl.step_y >= 0.0f) ||
				(edge12_tl.value + edge12_tl.step_x + edge12_tl.step_y >= 0.0f);
			bool edge20_inside =
				(edge20_tl.value >= 0.0f) ||
				(edge20_tl.value + edge20_tl.step_x >= 0.0f) ||
				(edge20_tl.value + edge20_tl.step_y >= 0.0f) ||
				(edge20_tl.value + edge20_tl.step_x + edge20_tl.step_y >= 0.0f);
			bool edge01_inside =
				(edge01_tl.value >= 0.0f) ||
				(edge01_tl.value + edge01_tl.step_x >= 0.0f) ||
				(edge01_tl.value + edge01_tl.step_y >= 0.0f) ||
				(edge01_tl.value + edge01_tl.step_x + edge01_tl.step_y >= 0.0f);
			bool bbox_included = edge12_inside && edge20_inside && edge01_inside;

			// Determine barycentric coordinates
			float w0 = edge12.value;
			float w1 = edge20.value;
			float w2 = edge01.value;

			// If p is on or inside all edges, render pixel
			//if ((w0 >= 0.0f && w1 >= 0.0f && w2 >= 0.0f))
			if (bbox_included)
			{
				// Normalize barycentric coordinates
				float alpha = w0 * rcp_wsum;
				float beta = w1 * rcp_wsum;
				float gamma = 1.0f - alpha - beta;

				// Clamp barycentric coordinates to stay within our face
				if (alpha < 0.0f)
				{
					beta += alpha * 0.5f;
					gamma += alpha * 0.5f;
					alpha = 0.0f;
				}
				if (beta < 0.0f)
				{
					alpha += beta * 0.5f;
					gamma += beta * 0.5f;
					beta = 0.0f;
				}
				if (gamma < 0.0f)
				{
					alpha += gamma * 0.5f;
					beta += gamma * 0.5f;
					gamma = 0.0f;
				}

				// Render pixel
				generate_face_fragment(lightmap, x, y, target_texture, alpha, beta, gamma, vertices, normals);
			}

			edge12.next_x();
			edge20.next_x();
			edge01.next_x();

			edge12_tl.next_x();
			edge20_tl.next_x();
			edge01_tl.next_x();
		}

		edge12.next_row();
		edge20.next_row();
		edge01.next_row();

		edge12_tl.next_row();
		edge20_tl.next_row();
		edge01_tl.next_row();
	}
}

void LightmapTexture::generate_face_fragment(std::shared_ptr<LightmapBuffers> &lightmap, int x, int y, int target_texture, float a, float b, float c, const Vec3f *vertices, const Vec3f *normals)
{
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

		PixelBufferPtr pixelbuffer = PixelBuffer::create(buffer->light.width(), buffer->light.height(), tf_rgb32f, buffer->light.data(), true);
		PNGFormat::save(pixelbuffer, texture.name);
	}
}
