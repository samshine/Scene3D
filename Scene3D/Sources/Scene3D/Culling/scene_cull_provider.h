
#pragma once

#include <vector>

class SceneItem
{
public:
	virtual ~SceneItem() { }
};

class SceneCullProxy
{
};

/// \brief Interface for finding the potential visible set (PVS) for a set of objects
class SceneCullProvider
{
public:
	virtual ~SceneCullProvider() { }

	virtual SceneCullProxy *create_proxy(SceneItem *item, const uicore::AxisAlignedBoundingBox &aabb) = 0;
	virtual void delete_proxy(SceneCullProxy *proxy) = 0;

	virtual void set_aabb(SceneCullProxy *proxy, const uicore::AxisAlignedBoundingBox &aabb) = 0;
	virtual uicore::AxisAlignedBoundingBox get_aabb(SceneCullProxy *proxy) = 0;

	virtual std::vector<SceneItem *> cull(const uicore::FrustumPlanes &frustum) = 0;
	virtual std::vector<SceneItem *> cull(const uicore::Vec3f &point) = 0;
};
