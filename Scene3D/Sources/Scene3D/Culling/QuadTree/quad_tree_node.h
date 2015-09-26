
#pragma once

#include "Scene3D/scene_cull_provider.h"

class QuadTreeNode
{
public:
	QuadTreeNode();
	~QuadTreeNode();
	void cull(int frame, const uicore::FrustumPlanes &frustum, const uicore::AxisAlignedBoundingBox &aabb, std::vector<SceneItem *> &pvs);

private:
	std::vector<SceneItem *> objects;
	QuadTreeNode *children[4];
};
