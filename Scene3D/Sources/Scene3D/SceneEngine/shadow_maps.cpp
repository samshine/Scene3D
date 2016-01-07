
#include "precomp.h"
#include "shadow_maps.h"
#include "scene_render.h"
#include "Scene3D/Scene/scene_camera_impl.h"

using namespace uicore;

ShadowMaps::ShadowMaps(SceneRender &render) : render(render)
{
}

void ShadowMaps::setup(int shadow_map_size, int max_active_maps, TextureFormat format)
{
	if (shadow_maps)
		return;

	shadow_maps = Texture2DArray::create(render.gc, shadow_map_size, shadow_map_size, max_active_maps, format);

	blur_texture = Texture2D::create(render.gc, shadow_map_size, shadow_map_size, format);
	fb_blur = FrameBuffer::create(render.gc);
	fb_blur->attach_color(0, blur_texture);

	auto depth_texture = Texture2D::create(render.gc, shadow_map_size, shadow_map_size, tf_depth_component32);
	for (int i = 0; i < max_active_maps; i++)
	{
		ShadowMapLight light;
		light.framebuffer = FrameBuffer::create(render.gc);
		light.framebuffer->attach_color(0, shadow_maps, i);
		light.framebuffer->attach_depth(depth_texture);
		light.view = shadow_maps->create_2d_view(i, format, 0, 1);
		lights.push_back(light);
	}
}

void ShadowMaps::start_frame()
{
	// Free all slots containing destroyed lights
	// Update camera distance for remaining lights
	for (auto &light : lights)
	{
		if (!light.light_weakptr.lock())
		{
			light.light = nullptr;
			light.light_weakptr.reset();
		}
		else
		{
			auto delta = light.light->position() - render.camera->position();
			light.sqr_distance = Vec3f::dot(delta, delta);
		}
	}
}

void ShadowMaps::add_light(SceneLightImpl *light)
{
	if (light->shadow_map_index != -1)
		return;

	auto delta = light->position() - render.camera->position();
	auto sqr_distance = Vec3f::dot(delta, delta);

	// Find light slot with furthest distance, or empty
	int slot = 0;
	for (int i = 0; i < (int)lights.size(); i++)
	{
		if (!lights[i].light)
		{
			slot = i;
			break;
		}
		else if (lights[i].sqr_distance > lights[slot].sqr_distance)
		{
			slot = i;
		}
	}

	if (!lights[slot].light || lights[slot].sqr_distance > sqr_distance)
	{
		if (lights[slot].light)
			lights[slot].light->shadow_map_index = -1;

		lights[slot].light = light;
		lights[slot].light_weakptr = light->shared_from_this();
		lights[slot].light->shadow_map_index = slot;
		lights[slot].sqr_distance = sqr_distance;
	}
}
