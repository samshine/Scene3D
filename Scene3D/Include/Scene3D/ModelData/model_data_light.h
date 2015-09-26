
#pragma once

/// \brief Description of a light in a model
class ModelDataLight
{
public:
	ModelDataAnimationData<uicore::Vec3f> position;
	ModelDataAnimationData<uicore::Quaternionf> orientation;
	ModelDataAnimationData<uicore::Vec3f> color;
	ModelDataAnimationData<float> attenuation_start;
	ModelDataAnimationData<float> attenuation_end;
	ModelDataAnimationData<float> falloff;
	ModelDataAnimationData<float> hotspot;
	ModelDataAnimationData<float> aspect;
	ModelDataAnimationData<float> ambient_illumination;
	int bone_selector = -1;
	bool casts_shadows = false;
	bool rectangle = false;
};
