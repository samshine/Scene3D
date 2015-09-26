
#pragma once

#include "model_data_animation_data.h"

/// \brief Texture sampler state and UVW animation data used when drawing vertex elements
class ModelDataTextureMap
{
public:
	enum WrapMode
	{
		wrap_repeat,
		wrap_clamp_to_edge,
		wrap_mirror
	};

	int texture = -1;
	int channel = -1;

	WrapMode wrap_x = wrap_repeat, wrap_y = wrap_repeat;

	ModelDataAnimationData<uicore::Vec3f> uvw_offset;
	ModelDataAnimationData<uicore::Quaternionf> uvw_rotation;
	ModelDataAnimationData<uicore::Vec3f> uvw_scale;

	uicore::Mat4f get_uvw_matrix(int animation_index, float animation_time)
	{
		if (channel != -1 && texture != -1)
			return
				uicore::Mat4f::translate(uvw_offset.get_value(animation_index, animation_time)) *
				uicore::Mat4f::scale(uvw_scale.get_value(animation_index, animation_time)) *
				uvw_rotation.get_value(animation_index, animation_time).to_matrix();
		else
			return uicore::Mat4f::identity();
	}
};
