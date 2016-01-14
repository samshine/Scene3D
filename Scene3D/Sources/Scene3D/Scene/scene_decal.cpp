
#include "precomp.h"
#include "Scene3D/scene_decal.h"
#include "Scene3D/scene.h"
#include "Scene3D/Scene/scene_decal_impl.h"
#include "Scene3D/Scene/scene_impl.h"

using namespace uicore;

std::shared_ptr<SceneDecal> SceneDecal::create(const ScenePtr &scene)
{
	return std::make_shared<SceneDecalImpl>(static_cast<SceneImpl*>(scene.get()));
}

SceneDecalImpl::SceneDecalImpl(SceneImpl *scene) : scene(scene)
{
	it = scene->decals.insert(scene->decals.end(), this);

	cull_proxy = scene->cull_provider->create_proxy(this, get_aabb());
}

SceneDecalImpl::~SceneDecalImpl()
{
	if (cull_proxy)
		scene->cull_provider->delete_proxy(cull_proxy);
	scene->decals.erase(it);
}

void SceneDecalImpl::set_position(const Vec3f &position)
{
	if (_position != position)
	{
		_position = position;
		if (cull_proxy)
			scene->cull_provider->set_aabb(cull_proxy, get_aabb());
	}
}

void SceneDecalImpl::set_orientation(const Quaternionf &orientation)
{
	_orientation = orientation;
}

void SceneDecalImpl::set_extents(const uicore::Vec3f &extents)
{
	if (_extents != extents)
	{
		_extents = extents;
		if (cull_proxy)
			scene->cull_provider->set_aabb(cull_proxy, get_aabb());
	}
}

AxisAlignedBoundingBox SceneDecalImpl::get_aabb()
{
	float max_extents = std::max(_extents.x, std::max(_extents.y, _extents.z));
	AxisAlignedBoundingBox aabb;
	aabb.aabb_min = _position - max_extents;
	aabb.aabb_max = _position + max_extents;
	return aabb;
}
