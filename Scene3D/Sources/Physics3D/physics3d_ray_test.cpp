
#include "precomp.h"
#include "Physics3D/physics3d_ray_test.h"
#include "Physics3D/physics3d_object.h"
#include "Physics3D/physics3d_world.h"
#include "physics3d_ray_test_impl.h"
#include "physics3d_world_impl.h"
#include "physics3d_object_impl.h"

std::shared_ptr<Physics3DRayTest> Physics3DRayTest::create(const Physics3DWorldPtr &world)
{
	return std::make_shared<Physics3DRayTestImpl>(static_cast<Physics3DWorldImpl*>(world.get()));
}

/////////////////////////////////////////////////////////////////////////////

Physics3DRayTestImpl::Physics3DRayTestImpl(Physics3DWorldImpl *world) : world(world)
{
}

Physics3DRayTestImpl::~Physics3DRayTestImpl()
{
}

bool Physics3DRayTestImpl::test(const uicore::Vec3f &new_start, const uicore::Vec3f &new_end)
{
	start = new_start;
	end = new_end;

	btVector3 bt_start(start.x, start.y, start.z);
	btVector3 bt_end(end.x, end.y, end.z);

	btCollisionWorld::ClosestRayResultCallback callback(bt_start, bt_end);
	world->dynamics_world->rayTest(bt_start, bt_end, callback);
	if (callback.hasHit())
	{
		_hit = true;
		_hit_fraction = callback.m_closestHitFraction;
		_hit_normal = uicore::Vec3f(callback.m_hitNormalWorld.x(), callback.m_hitNormalWorld.y(), callback.m_hitNormalWorld.z());
		_hit_object = static_cast<Physics3DObjectImpl*>(callback.m_collisionObject->getUserPointer());
	}
	else
	{
		_hit = false;
		_hit_fraction = 1.0f;
		_hit_normal = uicore::Vec3f();
		_hit_object = 0;
	}

	return _hit;
}

bool Physics3DRayTestImpl::hit() const
{
	return _hit;
}

float Physics3DRayTestImpl::hit_fraction() const
{
	return _hit_fraction;
}

uicore::Vec3f Physics3DRayTestImpl::hit_position() const
{
	return mix(start, end, _hit_fraction);
}

uicore::Vec3f Physics3DRayTestImpl::hit_normal() const
{
	return _hit_normal;
}

Physics3DObject Physics3DRayTestImpl::hit_object() const
{
	if (_hit_object)
		return Physics3DObject(_hit_object->shared_from_this());
	else
		return Physics3DObject();
}
