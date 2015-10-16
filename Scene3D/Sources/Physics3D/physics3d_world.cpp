
#include "precomp.h"
#include "Physics3D/physics3d_world.h"
#include "physics3d_world_impl.h"

using namespace uicore;

Physics3DWorld::Physics3DWorld()
	: impl(std::make_shared<Physics3DWorldImpl>())
{
}

bool Physics3DWorld::is_null() const
{
	return !impl;
}

int Physics3DWorld::step_simulation(float time_step, int max_sub_steps, float fixed_time_step)
{
	int num_sub_steps = impl->dynamics_world->stepSimulation(time_step, max_sub_steps, fixed_time_step);
	return num_sub_steps;
}

void Physics3DWorld::step_simulation_once(float time_step)
{
	int steps = impl->dynamics_world->stepSimulation(time_step, 0, time_step);
	if (steps == 0)
		throw Exception("btDynamicsWorld::stepSimulation failed");
}

void Physics3DWorld::set_gravity(const Vec3f &gravity)
{
	impl->dynamics_world->setGravity(btVector3(gravity.x, gravity.y, gravity.z));
}

/////////////////////////////////////////////////////////////////////////////

Physics3DWorldImpl::Physics3DWorldImpl()
{
	collision_configuration.reset(new btDefaultCollisionConfiguration());
	dispatcher.reset(new btCollisionDispatcher(collision_configuration.get()));
	broadphase.reset(new btDbvtBroadphase());
	ghost_pair_callback.reset(new btGhostPairCallback());
	broadphase->getOverlappingPairCache()->setInternalGhostPairCallback(ghost_pair_callback.get());
	constraint_solver.reset(new btSequentialImpulseConstraintSolver());
	dynamics_world.reset(new btDiscreteDynamicsWorld(dispatcher.get(), broadphase.get(), constraint_solver.get(), collision_configuration.get()));
	dynamics_world->getDispatchInfo().m_allowedCcdPenetration=0.0001f; // This line is needed by the character controller
}

Physics3DWorldImpl::~Physics3DWorldImpl()
{
	// To do: dispose all collision user objects
}
