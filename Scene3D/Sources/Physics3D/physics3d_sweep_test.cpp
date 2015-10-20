
#include "precomp.h"
#include "Physics3D/physics3d_sweep_test.h"
#include "Physics3D/physics3d_object.h"
#include "Physics3D/physics3d_shape.h"
#include "Physics3D/physics3d_world.h"
#include "physics3d_sweep_test_impl.h"
#include "physics3d_world_impl.h"
#include "physics3d_shape_impl.h"
#include "physics3d_object_impl.h"
#include <algorithm>

using namespace uicore;

std::shared_ptr<Physics3DSweepTest> Physics3DSweepTest::create(const Physics3DWorldPtr &world)
{
	return std::make_shared<Physics3DSweepTestImpl>(static_cast<Physics3DWorldImpl*>(world.get()));
}

bool Physics3DSweepTestImpl::test_any_hit(const Physics3DShapePtr &shape, const Vec3f &test_from_pos, const Quaternionf &from_orientation, const Vec3f &test_to_pos, const Quaternionf &to_orientation, float allowed_ccd_penetration)
{
	btConvexShape *convex_shape = dynamic_cast<btConvexShape*>(static_cast<Physics3DShapeImpl*>(shape.get())->shape.get());
	if (convex_shape == 0)
		throw Exception("Sweep testing is only supported for convex shapes");

	from_pos = test_from_pos;
	to_pos = test_to_pos;
	hits.clear();

	btVector3 bt_from_pos(from_pos.x, from_pos.y, from_pos.z);
	btVector3 bt_to_pos(to_pos.x, to_pos.y, to_pos.z);

	btTransform bt_from_transform(btQuaternion(from_orientation.x, from_orientation.y, from_orientation.z, from_orientation.w), bt_from_pos);
	btTransform bt_to_transform(btQuaternion(to_orientation.x, to_orientation.y, to_orientation.z, to_orientation.w), bt_to_pos);

	AnyHitConvexResultCallback callback(bt_from_pos, bt_to_pos);
	world->dynamics_world->convexSweepTest(convex_shape, bt_from_transform, bt_to_transform, callback, allowed_ccd_penetration);

	if (callback.hasHit())
	{
		SweepHit hit;
		hit.hit_fraction = callback.m_hitFraction;
		hit.hit_normal = Vec3f(callback.m_hitNormalWorld.getX(), callback.m_hitNormalWorld.getY(), callback.m_hitNormalWorld.getZ());
		hit.hit_collision_object = static_cast<Physics3DObjectImpl *>(callback.m_hitCollisionObject->getUserPointer());
		hits.push_back(hit);
	}

	return !hits.empty();
}

bool Physics3DSweepTestImpl::test_first_hit(const Physics3DShapePtr &shape, const Vec3f &test_from_pos, const Quaternionf &from_orientation, const Vec3f &test_to_pos, const Quaternionf &to_orientation, float allowed_ccd_penetration)
{
	btConvexShape *convex_shape = dynamic_cast<btConvexShape*>(static_cast<Physics3DShapeImpl*>(shape.get())->shape.get());
	if (convex_shape == 0)
		throw Exception("Sweep testing is only supported for convex shapes");

	from_pos = test_from_pos;
	to_pos = test_to_pos;
	hits.clear();

	btVector3 bt_from_pos(from_pos.x, from_pos.y, from_pos.z);
	btVector3 bt_to_pos(to_pos.x, to_pos.y, to_pos.z);

	btTransform bt_from_transform(btQuaternion(from_orientation.x, from_orientation.y, from_orientation.z, from_orientation.w), bt_from_pos);
	btTransform bt_to_transform(btQuaternion(to_orientation.x, to_orientation.y, to_orientation.z, to_orientation.w), bt_to_pos);

	btCollisionWorld::ClosestConvexResultCallback callback(bt_from_pos, bt_to_pos);
	world->dynamics_world->convexSweepTest(convex_shape, bt_from_transform, bt_to_transform, callback, allowed_ccd_penetration);

	if (callback.hasHit())
	{
		SweepHit hit;
		hit.hit_fraction = callback.m_closestHitFraction;
		hit.hit_normal = Vec3f(callback.m_hitNormalWorld.getX(), callback.m_hitNormalWorld.getY(), callback.m_hitNormalWorld.getZ());
		hit.hit_collision_object = static_cast<Physics3DObjectImpl *>(callback.m_hitCollisionObject->getUserPointer());
		hits.push_back(hit);
	}

	return !hits.empty();
}

bool Physics3DSweepTestImpl::test_all_hits(const Physics3DShapePtr &shape, const Vec3f &test_from_pos, const Quaternionf &from_orientation, const Vec3f &test_to_pos, const Quaternionf &to_orientation, float allowed_ccd_penetration)
{
	btConvexShape *convex_shape = dynamic_cast<btConvexShape*>(static_cast<Physics3DShapeImpl*>(shape.get())->shape.get());
	if (convex_shape == 0)
		throw Exception("Sweep testing is only supported for convex shapes");

	from_pos = test_from_pos;
	to_pos = test_to_pos;
	hits.clear();

	btVector3 bt_from_pos(from_pos.x, from_pos.y, from_pos.z);
	btVector3 bt_to_pos(to_pos.x, to_pos.y, to_pos.z);

	btTransform bt_from_transform(btQuaternion(from_orientation.x, from_orientation.y, from_orientation.z, from_orientation.w), bt_from_pos);
	btTransform bt_to_transform(btQuaternion(to_orientation.x, to_orientation.y, to_orientation.z, to_orientation.w), bt_to_pos);

	AllHitsConvexResultCallback callback(this);
	world->dynamics_world->convexSweepTest(convex_shape, bt_from_transform, bt_to_transform, callback, allowed_ccd_penetration);

	std::sort(hits.begin(), hits.end());

	return !hits.empty();
}

int Physics3DSweepTestImpl::hit_count() const
{
	return hits.size();
}

float Physics3DSweepTestImpl::hit_fraction(int index) const
{
	return hits[index].hit_fraction;
}

Vec3f Physics3DSweepTestImpl::hit_position(int index) const
{
	return mix(from_pos, to_pos, hits[index].hit_fraction);
}

Vec3f Physics3DSweepTestImpl::hit_normal(int index) const
{
	return hits[index].hit_normal;
}

Physics3DObject Physics3DSweepTestImpl::hit_object(int index) const
{
	if (!hits.empty())
		return Physics3DObject(hits[index].hit_collision_object->shared_from_this());
	else
		return Physics3DObject();
}

/////////////////////////////////////////////////////////////////////////////

Physics3DSweepTestImpl::Physics3DSweepTestImpl(Physics3DWorldImpl *world)
	: world(world)
{
}

Physics3DSweepTestImpl::~Physics3DSweepTestImpl()
{
}
