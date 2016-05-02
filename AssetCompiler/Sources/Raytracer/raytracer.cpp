
#include "precomp.h"
#include "raytracer.h"
#include "AssetCompiler/ModelDescription/model_desc.h"
#include "AssetCompiler/FBXModel/fbx_model.h"

using namespace uicore;

RayTracer::RayTracer(const MapDesc &desc)
{
	for (const auto &obj_desc : desc.objects)
	{
		auto &shape = shapes[obj_desc.mesh];
		auto &model_lights = mesh_lights[obj_desc.mesh];
		if (!shape)
		{
			try
			{
				auto model_desc = ModelDesc::load(obj_desc.mesh);
				auto fbx = FBXModel::load(model_desc.fbx_filename);
				auto model_data = fbx->convert(model_desc);
				shape = Physics3DShape::model(model_data);
				
				for (const auto &model_light : model_data->lights)
				{
					RayTracerLight light;
					light.position = model_light.position.get_single_value();
					light.color = model_light.color.get_single_value();
					light.attenuation_start = model_light.attenuation_start.get_single_value();
					light.attenuation_end = model_light.attenuation_end.get_single_value();
					model_lights.push_back(light);
				}
			}
			catch (...)
			{
				// To do: create dummy placeholder
				continue;
			}
		}

		Quaternionf rotation(obj_desc.up, obj_desc.dir, obj_desc.tilt, angle_degrees, order_YXZ);

		auto scaled_shape = shape;
		if (obj_desc.scale != 1.0f)
			scaled_shape = Physics3DShape::scale_model(shape, Vec3f(obj_desc.scale));

		auto collision_object = Physics3DObject::rigid_body(world, scaled_shape, 0.0f, obj_desc.position, rotation);
		objects.push_back(collision_object);
		for (auto light : model_lights)
		{
			light.position = obj_desc.position + rotation.rotate_vector(light.position) * obj_desc.scale;
			light.attenuation_start *= obj_desc.scale;
			light.attenuation_end *= obj_desc.scale;
			lights.push_back(light);
		}
	}
}

void RayTracer::generate_diffuse_gi(const std::string &filename)
{
	std::vector<PixelBufferPtr> pixelbuffers;
	for (int i = 0; i < 256; i++)
		pixelbuffers.push_back(PixelBuffer::create(256, 256, tf_rgba8));

	std::vector<std::future<void>> futures;
	int num_cores = std::max((int)std::thread::hardware_concurrency(), 8);
	for (int core = 0; core < num_cores; core++)
	{
		futures.push_back(std::async(std::launch::async, [&](int start_y)
		{
			for (int z = 0; z < 256; z++)
			{
				auto pixels = pixelbuffers[z]->data_uint32();
				for (int y = start_y; y < 256; y += num_cores)
				{
					for (int x = 0; x < 256; x++)
					{
						Vec3f position(x / 256.0f, y / 256.0f, z / 256.0f);
						position -= 0.5f;
						position *= 64.0f;

						Vec3f light = raytrace_diffuse_gi(position);

						uint32_t red = (uint32_t)std::max(std::min(light.x * 255, 255.0f), 0.0f);
						uint32_t green = (uint32_t)std::max(std::min(light.y * 255, 255.0f), 0.0f);
						uint32_t blue = (uint32_t)std::max(std::min(light.z * 255, 255.0f), 0.0f);

						uint32_t color = 0xff000000 | (blue << 16) | (green << 8) | red;
						pixels[x + y * 256] = color;
					}
				}
			}
		}, core));
	}
	for (auto &f : futures) f.get();

	auto pixelset = PixelBufferSet::create(texture_3d, tf_rgba8, 256, 256, 256);
	for (int i = 0; i < 256; i++)
		pixelset->set_image(i, 0, pixelbuffers[i]);

	//DDSFormat::save(pixelset);
}

Vec3f RayTracer::raytrace_diffuse_gi(const Vec3f &point) const
{
	Vec3f color;

	for (const auto &light : lights)
	{
		auto dir = light.position - point;
		auto length2 = Vec3f::dot(dir, dir);
		if (length2 > light.attenuation_end)
			continue;

		auto hit = world->ray_test_any(point, light.position);
		if (!hit)
		{
			color += light.color;
		}
	}

	return color;
}
