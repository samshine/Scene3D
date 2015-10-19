
#pragma once

class Particle
{
public:
	uicore::Vec3f position;
	uicore::Vec3f velocity;
	uicore::Vec3f acceleration;
	float life = -1.0;
	float life_speed = 0.0f;
	float start_size = 0.0f;
	float end_size = 0.0f;
};
