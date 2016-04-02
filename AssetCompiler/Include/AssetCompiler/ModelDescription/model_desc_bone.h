
#pragma once

#include <string>

class ModelDescBone
{
public:
	std::string name;
	std::string mesh_bone;
	std::string shape;
	uicore::Vec3f position;
	uicore::Quaternionf rotation;
	uicore::Vec3f extents;
};
