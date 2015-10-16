
#include "precomp.h"
#include "Physics3D/physics3d_contact_test.h"
#include "Physics3D/physics3d_object.h"
#include "Physics3D/physics3d_world.h"
#include "Physics3D/physics3d_shape.h"
#include "physics3d_contact_test_impl.h"
#include "physics3d_world_impl.h"
#include "physics3d_object_impl.h"
#include "physics3d_shape_impl.h"

using namespace uicore;

Physics3DContactTest::Physics3DContactTest()
{
}

Physics3DContactTest::Physics3DContactTest(Physics3DWorld &world)
	: impl(std::make_shared<Physics3DContactTestImpl>(world.impl.get()))
{
}

bool Physics3DContactTest::is_null() const
{
	return !impl;
}

bool Physics3DContactTest::test(const Physics3DObject &object)
{
	impl->contacts.clear();

	Physics3DContactTestImpl::AllHitsContactResultCallback callback(impl.get(), object.impl->object.get());
	impl->world->dynamics_world->contactTest(object.impl->object.get(), callback);

	return !impl->contacts.empty();
}

bool Physics3DContactTest::test(const Physics3DShape &shape, const Vec3f &position, const Quaternionf &orientation)
{
	btTransform transform(btQuaternion(orientation.x, orientation.y, orientation.z, orientation.w), btVector3(position.x, position.y, position.z));

	btCollisionObject obj;
	obj.setCollisionShape(shape.impl->shape.get());
	obj.setWorldTransform(transform);

	Physics3DContactTestImpl::AllHitsContactResultCallback callback(impl.get(), &obj);
	impl->world->dynamics_world->contactTest(&obj, callback);

	return !impl->contacts.empty();
}

int Physics3DContactTest::get_hit_count() const
{
	return impl->contacts.size();
}

Physics3DObject Physics3DContactTest::get_hit_object(int index) const
{
	if (!impl->contacts.empty())
		return Physics3DObject(impl->contacts[index].object->shared_from_this());
	else
		return Physics3DObject();
}

Vec3f Physics3DContactTest::get_hit_position(int index) const
{
	btVector3 &v = impl->contacts[index].hit_position;
	return Vec3f(v.x(), v.y(), v.z());
}

Vec3f Physics3DContactTest::get_hit_normal(int index) const
{
	btVector3 &v = impl->contacts[index].hit_normal;
	return Vec3f(v.x(), v.y(), v.z());
}

float Physics3DContactTest::get_hit_distance(int index) const
{
	return impl->contacts[index].hit_distance;
}


/////////////////////////////////////////////////////////////////////////////

Physics3DContactTestImpl::Physics3DContactTestImpl(Physics3DWorldImpl *world)
	: world(world)
{
}

Physics3DContactTestImpl::~Physics3DContactTestImpl()
{
}
