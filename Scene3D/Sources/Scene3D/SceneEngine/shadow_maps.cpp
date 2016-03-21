
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
	if (render.frames.front()->shadow_maps)
		return;

	render.frames.front()->shadow_maps = Texture2DArray::create(render.gc, shadow_map_size, shadow_map_size, max_active_maps, format);

	render.frames.front()->shadow_map_blur_texture = Texture2D::create(render.gc, shadow_map_size, shadow_map_size, format);
	render.frames.front()->fb_shadow_map_blur = FrameBuffer::create(render.gc);
	render.frames.front()->fb_shadow_map_blur->attach_color(0, render.frames.front()->shadow_map_blur_texture);

	auto depth_texture = Texture2D::create(render.gc, shadow_map_size, shadow_map_size, tf_depth_component32);
	for (int i = 0; i < max_active_maps; i++)
	{
		ShadowMapLight light;
		auto fb = FrameBuffer::create(render.gc);
		fb->attach_color(0, render.frames.front()->shadow_maps, i);
		fb->attach_depth(depth_texture);
		render.frames.front()->fb_shadow_map.push_back(fb);
		render.frames.front()->shadow_map_view.push_back(render.frames.front()->shadow_maps->create_2d_view(i, format, 0, 1));
	}

	lights.resize(max_active_maps);
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
