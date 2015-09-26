
#include "precomp.h"
#include "oct_tree.h"

using namespace uicore;

OctTree::OctTree()
: aabb(Vec3f(-300.0f), Vec3f(300.0f)), root(new OctTreeNode()), frame(0)
{
}

OctTree::OctTree(const AxisAlignedBoundingBox &aabb)
: aabb(aabb), root(new OctTreeNode()), frame(0)
{
}

OctTree::~OctTree()
{
	delete root;
}

SceneCullProxy *OctTree::create_proxy(SceneItem *object, const AxisAlignedBoundingBox &box)
{
	OctTreeObject *tree_object = new OctTreeObject(object, box);
	root->insert(tree_object, aabb);
	return tree_object;
}

void OctTree::delete_proxy(SceneCullProxy *proxy)
{
	OctTreeObject *tree_object = static_cast<OctTreeObject*>(proxy);
	root->remove(tree_object, aabb);
	tree_object->release();
}

void OctTree::set_aabb(SceneCullProxy *proxy, const AxisAlignedBoundingBox &box)
{
	OctTreeObject *tree_object = static_cast<OctTreeObject*>(proxy);
	root->remove(tree_object, aabb);
	tree_object->box = box;
	root->insert(tree_object, aabb);
}

AxisAlignedBoundingBox OctTree::get_aabb(SceneCullProxy *proxy)
{
	OctTreeObject *tree_object = static_cast<OctTreeObject*>(proxy);
	return tree_object->box;
}

std::vector<SceneItem *> OctTree::cull(const FrustumPlanes &frustum)
{
	std::vector<SceneItem *> pvs;
	root->cull(frame++, frustum, aabb, pvs);
	return pvs;
}

std::vector<SceneItem *> OctTree::cull(const Vec3f &point)
{
	std::vector<SceneItem *> pvs;
	root->cull(frame++, point, aabb, pvs);
	return pvs;
}
