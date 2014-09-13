
#pragma once

#include <ClanLib/core.h>

class ParticleEmitter
{
public:
	clan::Vec3f position;
	int bone_selector;
	float size;
	float speed;
	float spread;
	float gravity;
	float longevity;
	float delay;
	clan::Vec4f color;
	std::string texture;
};
