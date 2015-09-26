
#pragma once

#include "level_data_portal.h"
#include "level_data_sector.h"
#include "level_data_object.h"
#include "level_data_light.h"

class LevelData
{
public:
	std::vector<LevelDataPortal> portals;
	std::vector<LevelDataSector> sectors;
	std::vector<LevelDataObject> objects;
	std::vector<LevelDataLight> lights;
	std::vector<std::string> level_sets;
	uicore::Vec3f aabb_min, aabb_max;
};
