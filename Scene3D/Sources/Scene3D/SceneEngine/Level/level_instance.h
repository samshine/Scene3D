
#pragma once

class LevelInstance
{
public:
	uicore::Vec3f position;
	uicore::Quaternionf orientation;
	uicore::Vec3f scale;
	int level_set = 0;
};
