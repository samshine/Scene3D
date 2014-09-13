
#pragma once

#include <ClanLib/core.h>

class AttachmentPoint
{
public:
	std::string name;
	clan::Vec3f position;
	clan::Quaternionf orientation;
	std::string bone_name;
};
