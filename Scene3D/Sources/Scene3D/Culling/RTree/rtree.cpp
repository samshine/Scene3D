
#include "precomp.h"
#include "rtree.h"

using namespace uicore;

RTree::RTree()
: frame(0)
{
}

RTree::~RTree()
{
}

SceneCullProxy *RTree::create_proxy(SceneItem *object, const AxisAlignedBoundingBox &box)
{
	RTreeObject *tree_object = new RTreeObject(object, box);
	float a_min[3] = { box.aabb_min.x, box.aabb_min.y, box.aabb_min.z };
	float a_max[3] = { box.aabb_max.x, box.aabb_max.y, box.aabb_max.z };
	root.Insert(a_min, a_max, tree_object);
	return tree_object;
}

void RTree::delete_proxy(SceneCullProxy *proxy)
{
	RTreeObject *tree_object = static_cast<RTreeObject*>(proxy);

	float a_old_min[3] = { tree_object->box.aabb_min.x, tree_object->box.aabb_min.y, tree_object->box.aabb_min.z };
	float a_old_max[3] = { tree_object->box.aabb_max.x, tree_object->box.aabb_max.y, tree_object->box.aabb_max.z };

	root.Remove(a_old_min, a_old_max, tree_object);
	tree_object->release();
}

void RTree::set_aabb(SceneCullProxy *proxy, const AxisAlignedBoundingBox &box)
{
	RTreeObject *tree_object = static_cast<RTreeObject*>(proxy);

	float a_old_min[3] = { tree_object->box.aabb_min.x, tree_object->box.aabb_min.y, tree_object->box.aabb_min.z };
	float a_old_max[3] = { tree_object->box.aabb_max.x, tree_object->box.aabb_max.y, tree_object->box.aabb_max.z };

	float a_new_min[3] = { box.aabb_min.x, box.aabb_min.y, box.aabb_min.z };
	float a_new_max[3] = { box.aabb_max.x, box.aabb_max.y, box.aabb_max.z };

	root.Remove(a_old_min, a_old_max, tree_object);
	tree_object->box = box;
	root.Insert(a_new_min, a_new_max, tree_object);
}

AxisAlignedBoundingBox RTree::get_aabb(SceneCullProxy *proxy)
{
	RTreeObject *tree_object = static_cast<RTreeObject*>(proxy);
	return tree_object->box;
}

std::vector<SceneItem *> RTree::cull(const FrustumPlanes &frustum)
{
	std::vector<SceneItem *> pvs;
	//root->cull(frame++, frustum, aabb, pvs);
	return pvs;
}

std::vector<SceneItem *> RTree::cull(const Vec3f &point)
{
	std::vector<SceneItem *> pvs;
	return pvs;
}
