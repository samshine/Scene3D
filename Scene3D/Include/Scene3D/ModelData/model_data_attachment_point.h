
#pragma once

/// \brief Named attachment points of interest for a model
class ModelDataAttachmentPoint
{
public:
	std::string name;
	uicore::Vec3f position;
	uicore::Quaternionf orientation;
	int bone_selector = -1;
};
