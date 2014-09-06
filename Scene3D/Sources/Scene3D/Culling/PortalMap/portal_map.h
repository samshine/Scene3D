/*
**  ClanLib SDK
**  Copyright (c) 1997-2013 The ClanLib Team
**
**  This software is provided 'as-is', without any express or implied
**  warranty.  In no event will the authors be held liable for any damages
**  arising from the use of this software.
**
**  Permission is granted to anyone to use this software for any purpose,
**  including commercial applications, and to alter it and redistribute it
**  freely, subject to the following restrictions:
**
**  1. The origin of this software must not be misrepresented; you must not
**     claim that you wrote the original software. If you use this software
**     in a product, an acknowledgment in the product documentation would be
**     appreciated but is not required.
**  2. Altered source versions must be plainly marked as such, and must not be
**     misrepresented as being the original software.
**  3. This notice may not be removed or altered from any source distribution.
**
**  Note: Some of the libraries ClanLib may link to may have additional
**  requirements or restrictions.
**
**  File Author(s):
**
**    Magnus Norddahl
*/

#pragma once

namespace clan
{

class Portal;
class PortalSector;
class PortalClipping;
class SceneItem;
class FrustumPlanes;

class PortalMapObject
{
public:
	PortalMapObject(SceneItem *visible_object, const AxisAlignedBoundingBox &box) : ref_count(1), visible_object(visible_object), box(box), rendered_frame(-1) { }
	void add_ref() { ref_count++; }
	void release() { if (--ref_count == 0) delete this; }

	int ref_count;
	SceneItem *visible_object;
	AxisAlignedBoundingBox box;

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

	PortalMapObject *add_object(SceneItem *object, const AxisAlignedBoundingBox &box);
	void move_object(PortalMapObject *map_object, const AxisAlignedBoundingBox &box);
	void remove_object(PortalMapObject *map_object);

	std::vector<SceneItem *> cull(int frame, FrustumPlanes &frustum, const Mat4f &world_to_projection);

private:
	void cull_sector(const PortalClipping &clipping, PortalSector *sector, int frame, std::vector<SceneItem *> &pvs, const Rectf &box = Rectf(-1.0f, -1.0f, 1.0f, 1.0f));
	int find_camera_sector(const PortalClipping &clipping) const;
	bool is_in_sector(PortalSector *sector, const AxisAlignedBoundingBox &box) const;

	std::vector<PortalSector *> sectors;
	std::vector<Portal *> portals;
};

}

