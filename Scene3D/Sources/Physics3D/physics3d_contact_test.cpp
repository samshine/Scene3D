
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

std::shared_ptr<Physics3DContactTest> Physics3DContactTest::create(const Physics3DWorldPtr &world)
{
	return std::make_shared<Physics3DContactTestImpl>(static_cast<Physics3DWorldImpl*>(world.get()));
}

/////////////////////////////////////////////////////////////////////////////

Physics3DContactTestImpl::Physics3DContactTestImpl(Physics3DWorldImpl *world)
	: world(world)
{
}

Physics3DContactTestImpl::~Physics3DContactTestImpl()
{
}

bool Physics3DContactTestImpl::test(const Physics3DObjectPtr &object)
{
	contacts.clear();

	AllHitsContactResultCallback callback(this, static_cast<Physics3DObjectImpl*>(object.get())->object.get());
	world->dynamics_world->contactTest(static_cast<Physics3DObjectImpl*>(object.get())->object.get(), callback);

	return !contacts.empty();
}

bool Physics3DContactTestImpl::test(const Physics3DShapePtr &shape, const Vec3f &position, const Quaternionf &orientation)
{
	btTransform transform(btQuaternion(orientation.x, orientation.y, orientation.z, orientation.w), btVector3(position.x, position.y, position.z));

	btCollisionObject obj;
	obj.setCollisionShape(static_cast<Physics3DShapeImpl*>(shape.get())->shape.get());
	obj.setWorldTransform(transform);

	AllHitsContactResultCallback callback(this, &obj);
	world->dynamics_world->contactTest(&obj, callback);

	return !contacts.empty();
}

int Physics3DContactTestImpl::hit_count() const
{
	return contacts.size();
}

Physics3DObjectPtr Physics3DContactTestImpl::hit_object(int index) const
{
	if (!contacts.empty())
		return contacts[index].object->shared_from_this();
	else
		return nullptr;
}

Vec3f Physics3DContactTestImpl::hit_position(int index) const
{
	const btVector3 &v = contacts[index].hit_position;
	return Vec3f(v.x(), v.y(), v.z());
}

Vec3f Physics3DContactTestImpl::hit_normal(int index) const
{
	const btVector3 &v = contacts[index].hit_normal;
	return Vec3f(v.x(), v.y(), v.z());
}

float Physics3DContactTestImpl::hit_distance(int index) const
{
	return contacts[index].hit_distance;
}
