
#include "precomp.h"
#include "portal_map.h"
#include "portal_sector.h"
#include "portal.h"
#include "portal_clipping.h"
#include <algorithm>

using namespace uicore;

PortalMap::PortalMap()
{
}

PortalMap::~PortalMap()
{
	for (size_t i = 0; i < sectors.size(); i++)
	{
		for (size_t j = 0; j < sectors[i]->objects.size(); j++)
			sectors[i]->objects[j]->release();
		delete sectors[i];
	}
	for (size_t i = 0; i < portals.size(); i++)
		delete portals[i];
}

PortalMapObject *PortalMap::add_object(SceneItem *object, const AxisAlignedBoundingBox &box)
{
	PortalMapObject *map_object = new PortalMapObject(object, box);
	for (size_t i = 0; i < sectors.size(); i++)
	{
		if (is_in_sector(sectors[i], box))
		{
			sectors[i]->objects.push_back(map_object);
			map_object->add_ref();
		}
	}
	return map_object;
}

void PortalMap::move_object(PortalMapObject *map_object, const AxisAlignedBoundingBox &box)
{
	for (size_t i = 0; i < sectors.size(); i++)
	{
		if (is_in_sector(sectors[i], map_object->box))
		{
			sectors[i]->objects.erase(std::find(sectors[i]->objects.begin(), sectors[i]->objects.end(), map_object));
			map_object->release();
		}
	}

	map_object->box = box;

	for (size_t i = 0; i < sectors.size(); i++)
	{
		if (is_in_sector(sectors[i], box))
		{
			sectors[i]->objects.push_back(map_object);
			map_object->add_ref();
		}
	}
}

void PortalMap::remove_object(PortalMapObject *map_object)
{
	for (size_t i = 0; i < sectors.size(); i++)
	{
		if (is_in_sector(sectors[i], map_object->box))
		{
			sectors[i]->objects.erase(std::find(sectors[i]->objects.begin(), sectors[i]->objects.end(), map_object));
			map_object->release();
		}
	}
	map_object->release();
}

std::vector<SceneItem *> PortalMap::cull(int frame, FrustumPlanes &frustum, const Mat4f &world_to_projection)
{
	std::vector<SceneItem *> pvs;
	PortalClipping clipping(frustum, world_to_projection);
	int camera_sector = find_camera_sector(clipping);
	if (camera_sector != -1)
		cull_sector(clipping, sectors[camera_sector], frame, pvs);
	return pvs;
}

void PortalMap::cull_sector(const PortalClipping &clipping, PortalSector *sector, int frame, std::vector<SceneItem *> &pvs, const Rectf &box)
{
	for (size_t i = 0; i < sector->objects.size(); i++)
	{
		sector->objects[i]->add(frame, pvs);
	}

	for (size_t i = 0; i < sector->portals.size(); i++)
	{
		Rectf portal_box = clipping.intersect(box, sector->portals[i]->points);
		if (portal_box.size() != Sizef())
		{
			// To do: modify frustum clipping planes to only cover the portal box

			if (sector->portals[i]->front == sector && sector->portals[i]->back)
				cull_sector(clipping, sector->portals[i]->back, frame, pvs, portal_box);
			else if (sector->portals[i]->back == sector && sector->portals[i]->front)
				cull_sector(clipping, sector->portals[i]->front, frame, pvs, portal_box);
		}
	}
}

int PortalMap::find_camera_sector(const PortalClipping &clipping) const
{
	Vec4f camera_position = clipping.projection_to_world * Vec4f(0.0f, 0.0f, -1.0f, 1.0f);
	camera_position /= camera_position.w;
	for (size_t i = 0; i < sectors.size(); i++)
	{
		bool inside = true;
		for (size_t j = 0; inside && j < sectors[i]->portals.size(); j++)
		{
			float distance = Vec4f::dot3(sectors[i]->portals[j]->plane, camera_position) + sectors[i]->portals[j]->plane.w;
			if (sectors[i]->portals[j]->front == sectors[i])
			{
				inside = distance > 0.0f;
			}
			else if (sectors[i]->portals[j]->back == sectors[i] && distance <= 0.0f)
			{
				inside = distance <= 0.0f;
			}
		}
		if (inside)
		{
			return i;
		}
	}
	return -1;
}

bool PortalMap::is_in_sector(PortalSector *sector, const AxisAlignedBoundingBox &box) const
{
	for (size_t j = 0; j < sector->portals.size(); j++)
	{
		IntersectionTest::Result result = IntersectionTest::plane_aabb(sector->portals[j]->plane, box);
		bool overlaps = (result == IntersectionTest::intersecting);
		if (sector->portals[j]->front == sector && result == IntersectionTest::inside)
			overlaps = true;
		else if (sector->portals[j]->back == sector && result == IntersectionTest::outside)
			overlaps = true;
		if (!overlaps)
			return false;
	}
	return true;
}
