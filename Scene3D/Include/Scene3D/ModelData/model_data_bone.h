
#pragma once

#include "model_data_animation_data.h"

/// \brief Bone data.
class ModelDataBone
{
public:
	bool billboarded = false;
	short parent_bone = -1;
	ModelDataAnimationData<uicore::Vec3f> position;
	ModelDataAnimationData<uicore::Quaternionf> orientation; // Maybe use a Vec4ub version to save memory?
	uicore::Vec3f pivot;
};
