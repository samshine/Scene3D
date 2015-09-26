
#include "precomp.h"
#include "quad_tree.h"

using namespace uicore;

QuadTree::QuadTree()
: root(0), frame(0)
{
}

QuadTree::~QuadTree()
{
	delete root;
}

SceneCullProxy *QuadTree::create_proxy(SceneItem *item, const AxisAlignedBoundingBox &aabb)
{
	return 0;
}

void QuadTree::delete_proxy(SceneCullProxy *proxy)
{
}

void QuadTree::set_aabb(SceneCullProxy *proxy, const AxisAlignedBoundingBox &aabb)
{
}

AxisAlignedBoundingBox QuadTree::get_aabb(SceneCullProxy *proxy)
{
	return AxisAlignedBoundingBox();
}

std::vector<SceneItem *> QuadTree::cull(const FrustumPlanes &frustum)
{
	frame++;
	std::vector<SceneItem *> pvs;
	root->cull(frame, frustum, aabb, pvs);
	return pvs;
}

std::vector<SceneItem *> QuadTree::cull(const Vec3f &point)
{
	std::vector<SceneItem *> pvs;
	return pvs;
}
