
#pragma once

#include <memory>
#include "Physics3D/Bullet/btBulletDynamicsCommon.h"
#include "Physics3D/Bullet/BulletCollision/CollisionDispatch/btGhostObject.h"

class Physics3DWorldImpl
{
public:
	Physics3DWorldImpl();
	~Physics3DWorldImpl();

	std::unique_ptr<btDefaultCollisionConfiguration> collision_configuration;
	std::unique_ptr<btConstraintSolver> constraint_solver;
	std::unique_ptr<btCollisionDispatcher> dispatcher;
	std::unique_ptr<btBroadphaseInterface> broadphase;
	std::unique_ptr<btGhostPairCallback> ghost_pair_callback;
	std::unique_ptr<btDiscreteDynamicsWorld> dynamics_world;
};
