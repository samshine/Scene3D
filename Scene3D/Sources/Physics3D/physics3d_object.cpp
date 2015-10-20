
#include "precomp.h"
#include "Physics3D/physics3d_object.h"
#include "Physics3D/physics3d_shape.h"
#include "Physics3D/physics3d_constraint.h"
#include "Physics3D/physics3d_world.h"
#include "physics3d_object_impl.h"
#include "physics3d_shape_impl.h"
#include "physics3d_constraint_impl.h"
#include "physics3d_world_impl.h"

using namespace uicore;

std::shared_ptr<Physics3DObject> Physics3DObject::collision_body(const Physics3DWorldPtr &world, const Physics3DShapePtr &shape, const Vec3f &position, const Quaternionf &orientation)
{
	auto instance = std::make_shared<Physics3DObjectImpl>(static_cast<Physics3DWorldImpl*>(world.get()));

	btTransform transform(btQuaternion(orientation.x, orientation.y, orientation.z, orientation.w), btVector3(position.x, position.y, position.z));

	instance->shape = shape;
	instance->object.reset(new btCollisionObject());
	instance->object->setUserPointer(instance.get());
	instance->object->setCollisionShape(static_cast<Physics3DShapeImpl*>(shape.get())->shape.get());
	instance->object->setWorldTransform(transform);

	static_cast<Physics3DWorldImpl*>(world.get())->dynamics_world->addCollisionObject(instance->object.get());

	return instance;
}

std::shared_ptr<Physics3DObject> Physics3DObject::ghost_body(const Physics3DWorldPtr &world, const Physics3DShapePtr &shape, const Vec3f &position, const Quaternionf &orientation)
{
	auto instance = std::make_shared<Physics3DObjectImpl>(static_cast<Physics3DWorldImpl*>(world.get()));

	btTransform transform(btQuaternion(orientation.x, orientation.y, orientation.z, orientation.w), btVector3(position.x, position.y, position.z));

	instance->shape = shape;
	instance->object.reset(new btGhostObject());
	instance->object->setUserPointer(instance.get());
	instance->object->setCollisionShape(static_cast<Physics3DShapeImpl*>(shape.get())->shape.get());
	instance->object->setWorldTransform(transform);

	static_cast<Physics3DWorldImpl*>(world.get())->dynamics_world->addCollisionObject(instance->object.get());

	return instance;
}

std::shared_ptr<Physics3DObject> Physics3DObject::rigid_body(const Physics3DWorldPtr &world, const Physics3DShapePtr &shape, float mass, const Vec3f &position, const Quaternionf &orientation, const Vec3f &local_inertia)
{
	auto instance = std::make_shared<Physics3DObjectImpl>(static_cast<Physics3DWorldImpl*>(world.get()));

	btTransform transform(btQuaternion(orientation.x, orientation.y, orientation.z, orientation.w), btVector3(position.x, position.y, position.z));

	Vec3f inertia = local_inertia;
	if (mass != 0.0f && inertia == Vec3f())
	{
		btVector3 bt_inertia;
		static_cast<Physics3DShapeImpl*>(shape.get())->shape->calculateLocalInertia(mass, bt_inertia);
		inertia = Vec3f(bt_inertia.x(), bt_inertia.y(), bt_inertia.z());
	}

	instance->shape = shape;
	instance->object.reset(new btRigidBody(mass, 0, static_cast<Physics3DShapeImpl*>(shape.get())->shape.get(), btVector3(inertia.x, inertia.y, inertia.z)));
	instance->object->setUserPointer(instance.get());
	instance->object->setWorldTransform(transform);

	static_cast<Physics3DWorldImpl*>(world.get())->dynamics_world->addRigidBody(btRigidBody::upcast(instance->object.get()));

	return instance;
}

///////////////////////////////////////////////////////////////////////////

Physics3DObjectImpl::Physics3DObjectImpl(Physics3DWorldImpl *world)
	: world(world)
{
}

Physics3DObjectImpl::~Physics3DObjectImpl()
{
	if (object != 0)
	{
		btRigidBody *rigidBody = btRigidBody::upcast(object.get());
		if (rigidBody)
			world->dynamics_world->removeRigidBody(rigidBody);
		else
			world->dynamics_world->removeCollisionObject(object.get());
	}
}

Vec3f Physics3DObjectImpl::position() const
{
	btTransform transform = object->getWorldTransform();
	btVector3 origin = transform.getOrigin();
	return Vec3f(origin.getX(), origin.getY(), origin.getZ());
}

Quaternionf Physics3DObjectImpl::orientation() const
{
	btTransform transform = object->getWorldTransform();
	btQuaternion rotation = transform.getRotation();
	return Quaternionf(rotation.getW(), rotation.getX(), rotation.getY(), rotation.getZ());
}

bool Physics3DObjectImpl::static_object() const
{
	return (object->getCollisionFlags() & btCollisionObject::CF_STATIC_OBJECT) == btCollisionObject::CF_STATIC_OBJECT;
}

bool Physics3DObjectImpl::kinematic_object() const
{
	return (object->getCollisionFlags() & btCollisionObject::CF_KINEMATIC_OBJECT) == btCollisionObject::CF_KINEMATIC_OBJECT;
}

bool Physics3DObjectImpl::character_object() const
{
	return (object->getCollisionFlags() & btCollisionObject::CF_CHARACTER_OBJECT) == btCollisionObject::CF_CHARACTER_OBJECT;
}

bool Physics3DObjectImpl::debug_drawn() const
{
	return (object->getCollisionFlags() & btCollisionObject::CF_DISABLE_VISUALIZE_OBJECT) != btCollisionObject::CF_DISABLE_VISUALIZE_OBJECT;
}

void Physics3DObjectImpl::set_position(const Vec3f &position)
{
	btTransform transform = object->getWorldTransform();
	transform.setOrigin(btVector3(position.x, position.y, position.z));
	object->setWorldTransform(transform);
}

void Physics3DObjectImpl::set_orientation(const Quaternionf &orientation)
{
	btTransform transform = object->getWorldTransform();
	transform.setRotation(btQuaternion(orientation.x, orientation.y, orientation.z, orientation.w));
	object->setWorldTransform(transform);
}

void Physics3DObjectImpl::set_transform(const Vec3f &position, const Quaternionf &orientation)
{
	btTransform transform(btQuaternion(orientation.x, orientation.y, orientation.z, orientation.w), btVector3(position.x, position.y, position.z));
	object->setWorldTransform(transform);
}

void Physics3DObjectImpl::set_static_object(bool enable)
{
	if (enable)
		object->setCollisionFlags(object->getCollisionFlags() | btCollisionObject::CF_STATIC_OBJECT);
	else
		object->setCollisionFlags(object->getCollisionFlags() & (~btCollisionObject::CF_STATIC_OBJECT));
}

void Physics3DObjectImpl::set_kinematic_object(bool enable)
{
	if (enable)
		object->setCollisionFlags(object->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
	else
		object->setCollisionFlags(object->getCollisionFlags() & (~btCollisionObject::CF_KINEMATIC_OBJECT));
}

void Physics3DObjectImpl::set_character_object(bool enable)
{
	if (enable)
		object->setCollisionFlags(object->getCollisionFlags() | btCollisionObject::CF_CHARACTER_OBJECT);
	else
		object->setCollisionFlags(object->getCollisionFlags() & (~btCollisionObject::CF_CHARACTER_OBJECT));
}

void Physics3DObjectImpl::set_debug_drawn(bool enable)
{
	if (!enable)
		object->setCollisionFlags(object->getCollisionFlags() | btCollisionObject::CF_DISABLE_VISUALIZE_OBJECT);
	else
		object->setCollisionFlags(object->getCollisionFlags() & (~btCollisionObject::CF_DISABLE_VISUALIZE_OBJECT));
}

void Physics3DObjectImpl::set_mass(float mass, const Vec3f &local_inertia)
{
	btRigidBody *body = btRigidBody::upcast(object.get());
	if (body)
		body->setMassProps(mass, btVector3(local_inertia.x, local_inertia.y, local_inertia.z));
}

void Physics3DObjectImpl::set_sleeping_thresholds(float linear, float angular)
{
	btRigidBody *body = btRigidBody::upcast(object.get());
	if (body)
		body->setSleepingThresholds(linear, angular);
}

void Physics3DObjectImpl::set_ccd_swept_sphere_radius(float radius)
{
	btRigidBody *body = btRigidBody::upcast(object.get());
	if (body)
		body->setCcdSweptSphereRadius(radius);
}

void Physics3DObjectImpl::set_ccd_motion_threshold(float motion_threshold)
{
	btRigidBody *body = btRigidBody::upcast(object.get());
	if (body)
		body->setCcdMotionThreshold(motion_threshold);
}

void Physics3DObjectImpl::activate(bool force_activation)
{
	btRigidBody *body = btRigidBody::upcast(object.get());
	if (body)
		body->activate(force_activation);
}

void Physics3DObjectImpl::apply_central_force(const Vec3f &force)
{
	btRigidBody *body = btRigidBody::upcast(object.get());
	if (body)
	{
		body->activate();
		body->applyCentralForce(btVector3(force.x, force.y, force.z));
	}
}

void Physics3DObjectImpl::apply_torque(const Vec3f &torque)
{
	btRigidBody *body = btRigidBody::upcast(object.get());
	if (body)
	{
		body->activate();
		body->applyTorque(btVector3(torque.x, torque.y, torque.z));
	}
}

void Physics3DObjectImpl::apply_force(const Vec3f &force, const Vec3f &relative_pos)
{
	btRigidBody *body = btRigidBody::upcast(object.get());
	if (body)
	{
		body->activate();
		body->applyForce(btVector3(force.x, force.y, force.z), btVector3(relative_pos.x, relative_pos.y, relative_pos.z));
	}
}

void Physics3DObjectImpl::apply_central_impulse(const Vec3f &force)
{
	btRigidBody *body = btRigidBody::upcast(object.get());
	if (body)
	{
		body->activate();
		body->applyCentralImpulse(btVector3(force.x, force.y, force.z));
	}
}

void Physics3DObjectImpl::apply_torque_impulse(const Vec3f &torque)
{
	btRigidBody *body = btRigidBody::upcast(object.get());
	if (body)
	{
		body->activate();
		body->applyTorqueImpulse(btVector3(torque.x, torque.y, torque.z));
	}
}

void Physics3DObjectImpl::apply_impulse(const Vec3f &impulse, const Vec3f &relative_pos)
{
	btRigidBody *body = btRigidBody::upcast(object.get());
	if (body)
	{
		body->activate();
		body->applyImpulse(btVector3(impulse.x, impulse.y, impulse.z), btVector3(relative_pos.x, relative_pos.y, relative_pos.z));
	}
}

void Physics3DObjectImpl::clear_forces()
{
	btRigidBody *body = btRigidBody::upcast(object.get());
	if (body)
		body->clearForces();
}

void Physics3DObjectImpl::add_constraint(const Physics3DConstraintPtr &constraint)
{
	btRigidBody *body = btRigidBody::upcast(object.get());
	if (body)
		body->addConstraintRef(static_cast<Physics3DConstraintImpl*>(constraint.get())->constraint.get());
}

void Physics3DObjectImpl::remove_constraint(const Physics3DConstraintPtr &constraint)
{
	btRigidBody *body = btRigidBody::upcast(object.get());
	if (body)
		body->removeConstraintRef(static_cast<Physics3DConstraintImpl*>(constraint.get())->constraint.get());
}
