
#pragma once

#include "Scene3D/ModelData/model_data_draw_range.h"

class ModelDataDrawRange;

class ModelShaderDescription
{
public:
	ModelShaderDescription()
	: color_channel(), diffuse_channel(), specular_channel(), bumpmap_channel(), self_illumination_channel(), bones()
	{
	}

	ModelShaderDescription(const ModelDataDrawRange &description, bool use_bones, bool use_color_channel)
		: color_channel(use_color_channel), diffuse_channel(), specular_channel(), bumpmap_channel(), self_illumination_channel(), bones(use_bones)
	{
		diffuse_channel = (description.diffuse_map.texture != -1);
		specular_channel = (description.specular_map.texture != -1);
		bumpmap_channel = (description.bumpmap_map.texture != -1);
		self_illumination_channel = (description.self_illumination_map.texture != -1);
		lightmap_channel = (description.light_map.texture != -1);
	}

	bool operator<(const ModelShaderDescription &other) const
	{
		if (bones != other.bones)
			return bones < other.bones;
		else if (color_channel != other.color_channel)
			return color_channel < other.color_channel;
		else if (diffuse_channel != other.diffuse_channel)
			return diffuse_channel < other.diffuse_channel;
		else if (specular_channel != other.specular_channel)
			return specular_channel < other.specular_channel;
		else if (bumpmap_channel != other.bumpmap_channel)
			return bumpmap_channel < other.bumpmap_channel;
		else if (lightmap_channel != other.lightmap_channel)
			return lightmap_channel < other.lightmap_channel;
		else
			return self_illumination_channel < other.self_illumination_channel;
	}

	bool color_channel;
	bool diffuse_channel;
	bool specular_channel;
	bool bumpmap_channel;
	bool self_illumination_channel;
	bool lightmap_channel;
	bool bones;
};
