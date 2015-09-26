
#pragma once

#include "Scene3D/scene_cull_provider.h"

class OctTreeObject : public SceneCullProxy
{
public:
	OctTreeObject(SceneItem *visible_object, const uicore::AxisAlignedBoundingBox &box) : ref_count(1), visible_object(visible_object), box(box), rendered_frame(-1) { }
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

class OctTreeNode
{
public:
	OctTreeNode();
	~OctTreeNode();
	void insert(OctTreeObject *object, const uicore::AxisAlignedBoundingBox &aabb, int iteration = 0);
	void remove(OctTreeObject *object, const uicore::AxisAlignedBoundingBox &aabb, int iteration = 0);
	void cull(int frame, const uicore::FrustumPlanes &frustum, const uicore::AxisAlignedBoundingBox &aabb, std::vector<SceneItem *> &pvs);
	void cull(int frame, const uicore::Vec3f &point, const uicore::AxisAlignedBoundingBox &aabb, std::vector<SceneItem *> &pvs);
	void show(int frame, std::vector<SceneItem *> &pvs);

private:
	static uicore::AxisAlignedBoundingBox child_aabb(int index, const uicore::AxisAlignedBoundingBox &aabb);

	std::vector<OctTreeObject *> objects;
	OctTreeNode *children[8];
	static const uicore::Vec3f barycentric_weights[16];
	static const int max_iterations = 3;
};
