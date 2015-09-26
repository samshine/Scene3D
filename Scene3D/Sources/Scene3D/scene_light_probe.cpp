
#include "precomp.h"
#include "Scene3D/scene_light_probe.h"
#include "Scene3D/scene.h"
#include "scene_light_probe_impl.h"
#include "scene_impl.h"

using namespace uicore;

SceneLightProbe::SceneLightProbe()
{
}

SceneLightProbe::SceneLightProbe(Scene &scene)
	: impl(std::make_shared<SceneLightProbe_Impl>(scene.impl.get()))
{
	impl->cull_proxy = impl->scene->cull_provider->create_proxy(impl.get(), impl->get_aabb());
}

Vec3f SceneLightProbe::get_position() const
{
	return impl->position;
}

float SceneLightProbe::get_radius() const
{
	return impl->radius;
}

Vec3f SceneLightProbe::get_color() const
{
	return impl->color;
}

void SceneLightProbe::set_position(const Vec3f &position)
{
	if (impl->position != position)
	{
		impl->position = position;
		if (impl->cull_proxy)
			impl->scene->cull_provider->set_aabb(impl->cull_proxy, impl->get_aabb());
	}
}

void SceneLightProbe::set_radius(float radius)
{
	if (impl->radius != radius)
	{
		impl->radius = radius;
		if (impl->cull_proxy)
			impl->scene->cull_provider->set_aabb(impl->cull_proxy, impl->get_aabb());
	}
}

void SceneLightProbe::set_color(const Vec3f &color)
{
	impl->color = color;
}

/////////////////////////////////////////////////////////////////////////////

SceneLightProbe_Impl::SceneLightProbe_Impl(Scene_Impl *scene)
: scene(scene), cull_proxy(0), radius(1.0f)
{
	it = scene->light_probes.insert(scene->light_probes.end(), this);
}

SceneLightProbe_Impl::~SceneLightProbe_Impl()
{
	if (cull_proxy)
		scene->cull_provider->delete_proxy(cull_proxy);
	scene->light_probes.erase(it);
}

AxisAlignedBoundingBox SceneLightProbe_Impl::get_aabb()
{
	AxisAlignedBoundingBox aabb;
	aabb.aabb_min = position - radius * 1.73205081f;
	aabb.aabb_max = position + radius * 1.73205081f;
	return aabb;
}
