
#pragma once

#include "Scene3D/Culling/scene_cull_provider.h"
#include "oct_tree_node.h"

class OctTree : public SceneCullProvider
{
public:
	OctTree();
	OctTree(const uicore::AxisAlignedBoundingBox &aabb);
	~OctTree();

	SceneCullProxy *create_proxy(SceneItem *item, const uicore::AxisAlignedBoundingBox &aabb);
	void delete_proxy(SceneCullProxy *proxy);

	void set_aabb(SceneCullProxy *proxy, const uicore::AxisAlignedBoundingBox &aabb);
	uicore::AxisAlignedBoundingBox get_aabb(SceneCullProxy *proxy);

	std::vector<SceneItem *> cull(const uicore::FrustumPlanes &frustum);
	std::vector<SceneItem *> cull(const uicore::Vec3f &point);

private:
	uicore::AxisAlignedBoundingBox aabb;
	OctTreeNode *root;
	int frame;
};
