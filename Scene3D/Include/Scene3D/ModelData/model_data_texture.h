
#pragma once

/// \brief Description of a texture used by a model
class ModelDataTexture
{
public:
	ModelDataTexture() { }
	ModelDataTexture(const std::string &name, float gamma) : name(name), gamma(gamma) { }
	std::string name;
	float gamma = 1.0f;
};
