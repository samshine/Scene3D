
#include "precomp.h"
#include "Scene3D/scene_light.h"
#include "Scene3D/scene.h"
#include "Passes/VSMShadowMap/vsm_shadow_map_pass.h"
#include "Scene3D/scene_light_impl.h"
#include "Scene3D/scene_impl.h"

using namespace uicore;

std::shared_ptr<SceneLight> SceneLight::create(const ScenePtr &scene)
{
	return std::make_shared<SceneLightImpl>(static_cast<SceneImpl*>(scene.get()));
}

SceneLightImpl::SceneLightImpl(SceneImpl *scene) : scene(scene)
{
	it = scene->lights.insert(scene->lights.end(), this);

	cull_proxy = scene->cull_provider->create_proxy(this, get_aabb());
}

SceneLightImpl::~SceneLightImpl()
{
	if (cull_proxy)
		scene->cull_provider->delete_proxy(cull_proxy);
	scene->lights.erase(it);
}

void SceneLightImpl::set_position(const uicore::Vec3f &position)
{
	if (_position != position)
	{
		_position = position;
		if (cull_proxy)
			scene->cull_provider->set_aabb(cull_proxy, get_aabb());
	}
}

void SceneLightImpl::set_orientation(const uicore::Quaternionf &orientation)
{
	_orientation = orientation;
}

void SceneLightImpl::set_attenuation_start(float attenuation_start)
{
	if (_attenuation_start != attenuation_start)
	{
		_attenuation_start = attenuation_start;
		if (cull_proxy)
			scene->cull_provider->set_aabb(cull_proxy, get_aabb());
	}
}

void SceneLightImpl::set_attenuation_end(float attenuation_end)
{
	if (_attenuation_end != attenuation_end)
	{
		_attenuation_end = attenuation_end;
		if (cull_proxy)
			scene->cull_provider->set_aabb(cull_proxy, get_aabb());
	}
}

uicore::AxisAlignedBoundingBox SceneLightImpl::get_aabb()
{
	uicore::AxisAlignedBoundingBox aabb;
	aabb.aabb_min = _position - _attenuation_end * 1.73205081f;
	aabb.aabb_max = _position + _attenuation_end * 1.73205081f;
	return aabb;
}
