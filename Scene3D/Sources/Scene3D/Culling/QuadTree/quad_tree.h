
#pragma once

#include "quad_tree_node.h"

class QuadTree : public SceneCullProvider
{
public:
	QuadTree();
	~QuadTree();

	SceneCullProxy *create_proxy(SceneItem *item, const uicore::AxisAlignedBoundingBox &aabb);
	void delete_proxy(SceneCullProxy *proxy);

	void set_aabb(SceneCullProxy *proxy, const uicore::AxisAlignedBoundingBox &aabb);
	uicore::AxisAlignedBoundingBox get_aabb(SceneCullProxy *proxy);

	std::vector<SceneItem *> cull(const uicore::FrustumPlanes &frustum);
	std::vector<SceneItem *> cull(const uicore::Vec3f &point);

private:
	uicore::AxisAlignedBoundingBox aabb;
	QuadTreeNode *root;
	int frame;
};
