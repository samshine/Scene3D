
#include "precomp.h"
#include "Physics3D/physics3d_world.h"
#include "physics3d_world_impl.h"

using namespace uicore;

std::shared_ptr<Physics3DWorld> Physics3DWorld::create()
{
	return std::make_shared<Physics3DWorldImpl>();
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

int Physics3DWorldImpl::step_simulation(float time_step, int max_sub_steps, float fixed_time_step)
{
	int num_sub_steps = dynamics_world->stepSimulation(time_step, max_sub_steps, fixed_time_step);
	return num_sub_steps;
}

void Physics3DWorldImpl::step_simulation_once(float time_step)
{
	int steps = dynamics_world->stepSimulation(time_step, 0, time_step);
	if (steps == 0)
		throw Exception("btDynamicsWorld::stepSimulation failed");
}

void Physics3DWorldImpl::set_gravity(const Vec3f &gravity)
{
	dynamics_world->setGravity(btVector3(gravity.x, gravity.y, gravity.z));
}
