
#pragma once

#include <memory>

class Physics3DWorld
{
public:
	static std::shared_ptr<Physics3DWorld> create();

	virtual void set_gravity(const uicore::Vec3f &gravity) = 0;

	virtual int step_simulation(float time_step, int max_sub_steps, float fixed_time_step) = 0;
	virtual void step_simulation_once(float time_step) = 0;
};

typedef std::shared_ptr<Physics3DWorld> Physics3DWorldPtr;
