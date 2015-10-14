
#include "precomp.h"
#include "Scene3D/scene_light_probe.h"
#include "Scene3D/scene.h"
#include "scene_light_probe_impl.h"
#include "scene_impl.h"

using namespace uicore;

std::shared_ptr<SceneLightProbe> SceneLightProbe::create(Scene &scene)
{
	return std::make_shared<SceneLightProbe_Impl>(scene.impl.get());
}

SceneLightProbe_Impl::SceneLightProbe_Impl(Scene_Impl *scene)
: scene(scene)
{
	it = scene->light_probes.insert(scene->light_probes.end(), this);

	cull_proxy = scene->cull_provider->create_proxy(this, get_aabb());
}

SceneLightProbe_Impl::~SceneLightProbe_Impl()
{
	if (cull_proxy)
		scene->cull_provider->delete_proxy(cull_proxy);
	scene->light_probes.erase(it);
}

void SceneLightProbe_Impl::set_position(const Vec3f &position)
{
	if (_position != position)
	{
		_position = position;
		if (cull_proxy)
			scene->cull_provider->set_aabb(cull_proxy, get_aabb());
	}
}

void SceneLightProbe_Impl::set_radius(float radius)
{
	if (_radius != radius)
	{
		_radius = radius;
		if (cull_proxy)
			scene->cull_provider->set_aabb(cull_proxy, get_aabb());
	}
}

AxisAlignedBoundingBox SceneLightProbe_Impl::get_aabb()
{
	AxisAlignedBoundingBox aabb;
	aabb.aabb_min = _position - _radius * 1.73205081f;
	aabb.aabb_max = _position + _radius * 1.73205081f;
	return aabb;
}
