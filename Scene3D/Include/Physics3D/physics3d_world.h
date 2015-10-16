
#pragma once

#include <memory>

class Physics3DWorldImpl;

class Physics3DWorld
{
public:
	Physics3DWorld();

	bool is_null() const;

	void set_gravity(const uicore::Vec3f &gravity);

	int step_simulation(float time_step, int max_sub_steps, float fixed_time_step);
	void step_simulation_once(float time_step);

private:
	std::shared_ptr<Physics3DWorldImpl> impl;

	friend class Physics3DObject;
	friend class Physics3DShape;
	friend class Physics3DRayTest;
	friend class Physics3DSweepTest;
	friend class Physics3DContactTest;
	friend class Physics3DContactPairTest;
	friend class Physics3DConstraint;
};
