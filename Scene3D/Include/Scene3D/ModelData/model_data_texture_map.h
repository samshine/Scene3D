
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
};
