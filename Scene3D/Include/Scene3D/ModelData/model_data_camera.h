
#pragma once

#include "model_data_animation_data.h"

/// \brief Camera in the model
class ModelDataCamera
{
public:
	ModelDataAnimationData<uicore::Vec3f> position;
	ModelDataAnimationData<uicore::Quaternionf> orientation;
	float fov_y = 0.0f;
};
