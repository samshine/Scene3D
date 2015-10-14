
#pragma once

#include "Scene3D/Culling/scene_cull_provider.h"
#include "rtree_root.h"

class RTreeObject : public SceneCullProxy
{
public:
	RTreeObject(SceneItem *visible_object, const uicore::AxisAlignedBoundingBox &box) : ref_count(1), visible_object(visible_object), box(box), rendered_frame(-1) { }
	void add_ref() { ref_count++; }
	void release() { if (--ref_count == 0) delete this; }

	int ref_count;
	SceneItem *visible_object;
	uicore::AxisAlignedBoundingBox box;

	int rendered_frame;

	void add(int frame, std::vector<SceneItem *> &pvs)
	{
		if (rendered_frame != frame)
		{
			rendered_frame = frame;
			pvs.push_back(visible_object);
		}
	}
};

class RTree : public SceneCullProvider
{
public:
	RTree();
	~RTree();

	SceneCullProxy *create_proxy(SceneItem *item, const uicore::AxisAlignedBoundingBox &aabb);
	void delete_proxy(SceneCullProxy *proxy);

	void set_aabb(SceneCullProxy *proxy, const uicore::AxisAlignedBoundingBox &aabb);
	uicore::AxisAlignedBoundingBox get_aabb(SceneCullProxy *proxy);

	std::vector<SceneItem *> cull(const uicore::FrustumPlanes &frustum);
	std::vector<SceneItem *> cull(const uicore::Vec3f &point);

private:
	RTreeRoot<RTreeObject*, float, 3, float> root;
	int frame;
};
