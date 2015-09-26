
#pragma once

/// \brief Description of a particle emitter in a model
class ModelDataParticleEmitter
{
public:
	uicore::Vec3f position;
	int bone_selector = -1;
	float size = 0.0f;
	float speed = 0.0f;
	float spread = 0.0f;
	float gravity = 0.0f;
	float longevity = 0.0f;
	float delay = 0.0f;
	uicore::Vec4f color;
	std::string texture;
};
