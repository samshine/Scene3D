
#pragma once

#include <memory>
#include "Physics3D/physics3d_world.h"
#include "Physics3D/Bullet/btBulletDynamicsCommon.h"
#include "Physics3D/Bullet/BulletCollision/CollisionDispatch/btGhostObject.h"

class Physics3DWorldImpl : public Physics3DWorld
{
public:
	Physics3DWorldImpl();
	~Physics3DWorldImpl();

	void set_gravity(const uicore::Vec3f &gravity) override;
	int step_simulation(float time_step, int max_sub_steps, float fixed_time_step) override;
	void step_simulation_once(float time_step) override;

	std::unique_ptr<btDefaultCollisionConfiguration> collision_configuration;
	std::unique_ptr<btConstraintSolver> constraint_solver;
	std::unique_ptr<btCollisionDispatcher> dispatcher;
	std::unique_ptr<btBroadphaseInterface> broadphase;
	std::unique_ptr<btGhostPairCallback> ghost_pair_callback;
	std::unique_ptr<btDiscreteDynamicsWorld> dynamics_world;
};
