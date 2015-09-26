
#include "precomp.h"
#include "Physics3D/physics3d_ray_test.h"
#include "Physics3D/physics3d_object.h"
#include "Physics3D/physics3d_world.h"
#include "physics3d_ray_test_impl.h"
#include "physics3d_world_impl.h"
#include "physics3d_object_impl.h"

Physics3DRayTest::Physics3DRayTest()
{
}

Physics3DRayTest::Physics3DRayTest(Physics3DWorld &world)
	: impl(std::make_shared<Physics3DRayTest_Impl>(world.impl.get()))
{
}

bool Physics3DRayTest::is_null() const
{
	return !impl;
}

bool Physics3DRayTest::test(const uicore::Vec3f &new_start, const uicore::Vec3f &new_end)
{
	impl->start = new_start;
	impl->end = new_end;

	btVector3 bt_start(impl->start.x, impl->start.y, impl->start.z);
	btVector3 bt_end(impl->end.x, impl->end.y, impl->end.z);

	btCollisionWorld::ClosestRayResultCallback callback(bt_start, bt_end);
	impl->world->dynamics_world->rayTest(bt_start, bt_end, callback);
	if (callback.hasHit())
	{
		impl->has_hit = true;
		impl->hit_fraction = callback.m_closestHitFraction;
		impl->hit_normal = uicore::Vec3f(callback.m_hitNormalWorld.x(), callback.m_hitNormalWorld.y(), callback.m_hitNormalWorld.z());
		impl->hit_object = static_cast<Physics3DObject_Impl*>(callback.m_collisionObject->getUserPointer());
	}
	else
	{
		impl->has_hit = false;
		impl->hit_fraction = 1.0f;
		impl->hit_normal = uicore::Vec3f();
		impl->hit_object = 0;
	}

	return impl->has_hit;
}

bool Physics3DRayTest::has_hit() const
{
	return impl->has_hit;
}

float Physics3DRayTest::get_hit_fraction() const
{
	return impl->hit_fraction;
}

uicore::Vec3f Physics3DRayTest::get_hit_position() const
{
	return mix(impl->start, impl->end, impl->hit_fraction);
}

uicore::Vec3f Physics3DRayTest::get_hit_normal() const
{
	return impl->hit_normal;
}

Physics3DObject Physics3DRayTest::get_hit_object() const
{
	if (impl->hit_object)
		return Physics3DObject(impl->hit_object->shared_from_this());
	else
		return Physics3DObject();
}

/////////////////////////////////////////////////////////////////////////////

Physics3DRayTest_Impl::Physics3DRayTest_Impl(Physics3DWorld_Impl *world)
	: world(world), has_hit(false), hit_fraction(1.0f), hit_object(0)
{
}

Physics3DRayTest_Impl::~Physics3DRayTest_Impl()
{
}
