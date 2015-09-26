
#pragma once

#include "model_data_texture_map.h"
#include "model_data_animation_data.h"

/// \brief Information needed to draw a range of vertex elements in a ModelDataMesh.
class ModelDataDrawRange
{
public:
	/// \brief First vertex element to draw
	int start_element = 0;

	/// \brief Number of elements to draw
	int num_elements = 0;

	/// \brief Toggles if back facing faces should be drawn too
	bool two_sided = false;

	/// \brief Controls if faces should be rendered with alpha blending enabled
	bool transparent = false;

	/// \brief Must be set to true if the faces are rendered with alpha test transparency
	bool alpha_test = false;

	ModelDataAnimationData<uicore::Vec3f> ambient;
	ModelDataAnimationData<uicore::Vec3f> diffuse;
	ModelDataAnimationData<uicore::Vec3f> specular;
	ModelDataAnimationData<float> self_illumination_amount;
	ModelDataAnimationData<uicore::Vec3f> self_illumination;
	ModelDataAnimationData<float> glossiness;
	ModelDataAnimationData<float> specular_level;

	ModelDataTextureMap diffuse_map;
	ModelDataTextureMap specular_map;
	ModelDataTextureMap bumpmap_map;
	ModelDataTextureMap self_illumination_map;
	ModelDataTextureMap light_map;
};
