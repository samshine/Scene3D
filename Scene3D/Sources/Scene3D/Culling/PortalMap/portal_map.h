
#pragma once

class Portal;
class PortalSector;
class PortalClipping;
class SceneItem;

class PortalMapObject
{
public:
	PortalMapObject(SceneItem *visible_object, const uicore::AxisAlignedBoundingBox &box) : ref_count(1), visible_object(visible_object), box(box), rendered_frame(-1) { }
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

class PortalMap
{
public:
	PortalMap();
	~PortalMap();

	PortalMapObject *add_object(SceneItem *object, const uicore::AxisAlignedBoundingBox &box);
	void move_object(PortalMapObject *map_object, const uicore::AxisAlignedBoundingBox &box);
	void remove_object(PortalMapObject *map_object);

	std::vector<SceneItem *> cull(int frame, uicore::FrustumPlanes &frustum, const uicore::Mat4f &world_to_projection);

private:
	void cull_sector(const PortalClipping &clipping, PortalSector *sector, int frame, std::vector<SceneItem *> &pvs, const uicore::Rectf &box = uicore::Rectf(-1.0f, -1.0f, 1.0f, 1.0f));
	int find_camera_sector(const PortalClipping &clipping) const;
	bool is_in_sector(PortalSector *sector, const uicore::AxisAlignedBoundingBox &box) const;

	std::vector<PortalSector *> sectors;
	std::vector<Portal *> portals;
};
