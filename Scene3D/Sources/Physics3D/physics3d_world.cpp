
#include "precomp.h"
#include "Physics3D/physics3d_world.h"
#include "physics3d_world_impl.h"
#include "physics3d_shape_impl.h"

using namespace uicore;

std::shared_ptr<Physics3DWorld> Physics3DWorld::create()
{
	return std::make_shared<Physics3DWorldImpl>();
}

Physics3DHit Physics3DWorld::ray_test_nearest(const uicore::Vec3f &start, const uicore::Vec3f &end, const std::function<bool(const Physics3DHit &result)> &hit_predicate)
{
	Physics3DHit final_result;
	ray_test(start, end, [&](const Physics3DHit &result)
	{
		if (!hit_predicate || hit_predicate(result))
			final_result = result;
		return final_result.fraction;
	});
	return final_result;
}

Physics3DHit Physics3DWorld::ray_test_any(const uicore::Vec3f &start, const uicore::Vec3f &end, const std::function<bool(const Physics3DHit &result)> &hit_predicate)
{
	Physics3DHit final_result;
	ray_test(start, end, [&](const Physics3DHit &result)
	{
		if (!hit_predicate || hit_predicate(result))
			final_result = result;
		return final_result ? 0.0f : 1.0f;
	});
	return final_result;
}

std::vector<Physics3DHit> Physics3DWorld::ray_test_all(const uicore::Vec3f &start, const uicore::Vec3f &end, const std::function<bool(const Physics3DHit &result)> &hit_predicate)
{
	std::vector<Physics3DHit> final_result;
	ray_test(start, end, [&](const Physics3DHit &result)
	{
		if (!hit_predicate || hit_predicate(result))
			final_result.push_back(result);
		return 1.0f;
	});
	return final_result;
}

std::vector<Physics3DHit> Physics3DWorld::ray_test_all_sorted(const uicore::Vec3f &start, const uicore::Vec3f &end, const std::function<bool(const Physics3DHit &result)> &hit_predicate)
{
	auto hits = ray_test_all(start, end, hit_predicate);
	std::sort(hits.begin(), hits.end());
	return hits;
}

Physics3DHit Physics3DWorld::sweep_test_nearest(const Physics3DShapePtr &shape, const uicore::Vec3f &from_pos, const uicore::Quaternionf &from_orientation, const uicore::Vec3f &to_pos, const uicore::Quaternionf &to_orientation, float allowed_ccd_penetration, const std::function<bool(const Physics3DHit &result)> &hit_predicate)
{
	Physics3DHit final_result;
	sweep_test(shape, from_pos, from_orientation, to_pos, to_orientation, allowed_ccd_penetration, [&](const Physics3DHit &result)
	{
		if (!hit_predicate || hit_predicate(result))
			final_result = result;
		return final_result.fraction;
	});
	return final_result;
}

Physics3DHit Physics3DWorld::sweep_test_any(const Physics3DShapePtr &shape, const uicore::Vec3f &from_pos, const uicore::Quaternionf &from_orientation, const uicore::Vec3f &to_pos, const uicore::Quaternionf &to_orientation, float allowed_ccd_penetration, const std::function<bool(const Physics3DHit &result)> &hit_predicate)
{
	Physics3DHit final_result;
	sweep_test(shape, from_pos, from_orientation, to_pos, to_orientation, allowed_ccd_penetration, [&](const Physics3DHit &result)
	{
		if (!hit_predicate || hit_predicate(result))
			final_result = result;
		return final_result ? 0.0f : 1.0f;
	});
	return final_result;
}

std::vector<Physics3DHit> Physics3DWorld::sweep_test_all(const Physics3DShapePtr &shape, const uicore::Vec3f &from_pos, const uicore::Quaternionf &from_orientation, const uicore::Vec3f &to_pos, const uicore::Quaternionf &to_orientation, float allowed_ccd_penetration, const std::function<bool(const Physics3DHit &result)> &hit_predicate)
{
	std::vector<Physics3DHit> final_result;
	sweep_test(shape, from_pos, from_orientation, to_pos, to_orientation, allowed_ccd_penetration, [&](const Physics3DHit &result)
	{
		if (!hit_predicate || hit_predicate(result))
			final_result.push_back(result);
		return 1.0f;
	});
	return final_result;
}

std::vector<Physics3DHit> Physics3DWorld::sweep_test_all_sorted(const Physics3DShapePtr &shape, const uicore::Vec3f &from_pos, const uicore::Quaternionf &from_orientation, const uicore::Vec3f &to_pos, const uicore::Quaternionf &to_orientation, float allowed_ccd_penetration, const std::function<bool(const Physics3DHit &result)> &hit_predicate)
{
	auto hits = sweep_test_all(shape, from_pos, from_orientation, to_pos, to_orientation, allowed_ccd_penetration, hit_predicate);
	std::sort(hits.begin(), hits.end());
	return hits;
}

Physics3DContact Physics3DWorld::contact_test_any(const Physics3DObjectPtr &object, const std::function<bool(const Physics3DContact &result)> &contact_predicate)
{
	Physics3DContact final_result;
	contact_test(object, [&](const Physics3DContact &result)
	{
		if (!contact_predicate || contact_predicate(result))
			final_result = result;
		return final_result ? 0.0f : 1.0f;
	});
	return final_result;
}

Physics3DContact Physics3DWorld::contact_test_any(const Physics3DShapePtr &shape, const uicore::Vec3f &position, const uicore::Quaternionf &orientation, const std::function<bool(const Physics3DContact &result)> &contact_predicate)
{
	Physics3DContact final_result;
	contact_test(shape, position, orientation, [&](const Physics3DContact &result)
	{
		if (!contact_predicate || contact_predicate(result))
			final_result = result;
		return final_result ? 0.0f : 1.0f;
	});
	return final_result;
}

std::vector<Physics3DContact> Physics3DWorld::contact_test_all(const Physics3DObjectPtr &object, const std::function<bool(const Physics3DContact &result)> &contact_predicate)
{
	std::vector<Physics3DContact> final_result;
	contact_test(object, [&](const Physics3DContact &result)
	{
		if (!contact_predicate || contact_predicate(result))
			final_result.push_back(result);
		return 1.0f;
	});
	return final_result;
}

std::vector<Physics3DContact> Physics3DWorld::contact_test_all(const Physics3DShapePtr &shape, const uicore::Vec3f &position, const uicore::Quaternionf &orientation, const std::function<bool(const Physics3DContact &result)> &contact_predicate)
{
	std::vector<Physics3DContact> final_result;
	contact_test(shape, position, orientation, [&](const Physics3DContact &result)
	{
		if (!contact_predicate || contact_predicate(result))
			final_result.push_back(result);
		return 1.0f;
	});
	return final_result;
}

Physics3DContact Physics3DWorld::contact_pair_test_any(const Physics3DObjectPtr &object1, const Physics3DObjectPtr &object2, const std::function<bool(const Physics3DContact &result)> &contact_predicate)
{
	Physics3DContact final_result;
	contact_pair_test(object1, object2, [&](const Physics3DContact &result)
	{
		if (!contact_predicate || contact_predicate(result))
			final_result = result;
		return final_result ? 0.0f : 1.0f;
	});
	return final_result;
}

std::vector<Physics3DContact> Physics3DWorld::contact_pair_test_all(const Physics3DObjectPtr &object1, const Physics3DObjectPtr &object2, const std::function<bool(const Physics3DContact &result)> &contact_predicate)
{
	std::vector<Physics3DContact> final_result;
	contact_pair_test(object1, object2, [&](const Physics3DContact &result)
	{
		if (!contact_predicate || contact_predicate(result))
			final_result.push_back(result);
		return 1.0f;
	});
	return final_result;
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

void Physics3DWorldImpl::ray_test(const uicore::Vec3f &start, const uicore::Vec3f &end, const std::function<float(const Physics3DHit &result)> &hit_callback)
{
	btVector3 bt_start(start.x, start.y, start.z);
	btVector3 bt_end(end.x, end.y, end.z);

	Physics3DRayResultCallback cb(start, end, hit_callback);
	dynamics_world->rayTest(bt_start, bt_end, cb);
}

void Physics3DWorldImpl::sweep_test(const Physics3DShapePtr &shape, const uicore::Vec3f &from_pos, const uicore::Quaternionf &from_orientation, const uicore::Vec3f &to_pos, const uicore::Quaternionf &to_orientation, float allowed_ccd_penetration, const std::function<float(const Physics3DHit &result)> &hit_callback)
{
	btConvexShape *convex_shape = dynamic_cast<btConvexShape*>(static_cast<Physics3DShapeImpl*>(shape.get())->shape.get());
	if (!convex_shape)
		throw Exception("Sweep testing is only supported for convex shapes");

	btVector3 bt_from_pos(from_pos.x, from_pos.y, from_pos.z);
	btVector3 bt_to_pos(to_pos.x, to_pos.y, to_pos.z);

	btTransform bt_from_transform(btQuaternion(from_orientation.x, from_orientation.y, from_orientation.z, from_orientation.w), bt_from_pos);
	btTransform bt_to_transform(btQuaternion(to_orientation.x, to_orientation.y, to_orientation.z, to_orientation.w), bt_to_pos);

	Physics3DSweepResultCallback cb(from_pos, to_pos, hit_callback);
	dynamics_world->convexSweepTest(convex_shape, bt_from_transform, bt_to_transform, cb, allowed_ccd_penetration);
}

void Physics3DWorldImpl::contact_test(const Physics3DObjectPtr &object, const std::function<float(const Physics3DContact &result)> &contact_callback)
{
	Physics3DContactResultCallback cb(static_cast<Physics3DObjectImpl*>(object.get())->object.get(), contact_callback);
	dynamics_world->contactTest(static_cast<Physics3DObjectImpl*>(object.get())->object.get(), cb);
}

void Physics3DWorldImpl::contact_test(const Physics3DShapePtr &shape, const uicore::Vec3f &position, const uicore::Quaternionf &orientation, const std::function<float(const Physics3DContact &result)> &contact_callback)
{
	btTransform transform(btQuaternion(orientation.x, orientation.y, orientation.z, orientation.w), btVector3(position.x, position.y, position.z));

	btCollisionObject obj;
	obj.setCollisionShape(static_cast<Physics3DShapeImpl*>(shape.get())->shape.get());
	obj.setWorldTransform(transform);

	Physics3DContactResultCallback cb(&obj, contact_callback);
	dynamics_world->contactTest(&obj, cb);
}

void Physics3DWorldImpl::contact_pair_test(const Physics3DObjectPtr &object1, const Physics3DObjectPtr &object2, const std::function<float(const Physics3DContact &result)> &contact_callback)
{
	Physics3DContactResultCallback cb(static_cast<Physics3DObjectImpl*>(object1.get())->object.get(), contact_callback);
	dynamics_world->contactPairTest(static_cast<Physics3DObjectImpl*>(object1.get())->object.get(), static_cast<Physics3DObjectImpl*>(object2.get())->object.get(), cb);
}
