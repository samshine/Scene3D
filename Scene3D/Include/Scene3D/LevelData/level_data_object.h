
#pragma once

class LevelDataObject
{
public:
	std::string model_filename;
	uicore::Vec3f position;
	uicore::Quaternionf orientation;
	uicore::Vec3f scale;
	int level_set = 0;
};
