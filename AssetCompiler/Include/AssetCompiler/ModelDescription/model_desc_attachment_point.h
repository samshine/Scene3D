
#pragma once

class ModelDescAttachmentPoint
{
public:
	std::string name;
	uicore::Vec3f position;
	uicore::Quaternionf orientation;
	std::string bone_name;
	std::string test_model;
	float test_scale = 1.0f;
};
