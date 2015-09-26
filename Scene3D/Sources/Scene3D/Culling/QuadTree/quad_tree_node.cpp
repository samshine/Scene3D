
#include "precomp.h"
#include "quad_tree_node.h"

using namespace uicore;

QuadTreeNode::QuadTreeNode()
{
	for (int i = 0; i < 4; i++)
		children[i] = 0;
}

QuadTreeNode::~QuadTreeNode()
{
	for (int i = 0; i < 4; i++)
		delete children[i];
}

void QuadTreeNode::cull(int frame, const FrustumPlanes &frustum, const AxisAlignedBoundingBox &aabb, std::vector<SceneItem *> &pvs)
{
/*	if (frustum.is_visible(aabb))
	{
		for (size_t i = 0; i < objects.size(); i++)
			objects[i]->add(frame, pvs);

		children[0]->cull(frame, frustum, AxisAlignedBoundingBox(aabb, Vec3f(0.0f, 0.0f, 0.0f), Vec3f(0.5f, 1.0f, 0.5f)), pvs);
		children[1]->cull(frame, frustum, AxisAlignedBoundingBox(aabb, Vec3f(0.5f, 0.0f, 0.0f), Vec3f(1.0f, 1.0f, 0.5f)), pvs);
		children[2]->cull(frame, frustum, AxisAlignedBoundingBox(aabb, Vec3f(0.0f, 0.0f, 0.5f), Vec3f(0.5f, 1.0f, 1.0f)), pvs);
		children[3]->cull(frame, frustum, AxisAlignedBoundingBox(aabb, Vec3f(0.5f, 0.0f, 0.5f), Vec3f(1.0f, 1.0f, 1.0f)), pvs);
	}*/
}
