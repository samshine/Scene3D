
#include "precomp.h"
#include "Physics3D/physics3d_constraint.h"
#include "Physics3D/physics3d_object.h"
#include "Physics3D/physics3d_world.h"
#include "physics3d_constraint_impl.h"
#include "physics3d_object_impl.h"
#include "physics3d_world_impl.h"

using namespace uicore;

std::shared_ptr<Physics3DConstraint> Physics3DConstraint::point_to_point(const Physics3DWorldPtr &world, const Physics3DObjectPtr &a, const Vec3f &pivot_in_a)
{
	btRigidBody &body_a = *btRigidBody::upcast(static_cast<Physics3DObjectImpl*>(a.get())->object.get());

	auto constraint = std::make_shared<Physics3DConstraintImpl>(static_cast<Physics3DWorldImpl*>(world.get()));
	constraint->constraint.reset(new btPoint2PointConstraint(body_a, btVector3(pivot_in_a.x, pivot_in_a.y, pivot_in_a.z)));
	constraint->constraint->setUserConstraintPtr(constraint.get());

	static_cast<Physics3DWorldImpl*>(world.get())->dynamics_world->addConstraint(constraint->constraint.get());

	return constraint;
}

std::shared_ptr<Physics3DConstraint> Physics3DConstraint::point_to_point(const Physics3DWorldPtr &world, const Physics3DObjectPtr &a, const Physics3DObjectPtr &b, const Vec3f &pivot_in_a, const Vec3f &pivot_in_b)
{
	btRigidBody &body_a = *btRigidBody::upcast(static_cast<Physics3DObjectImpl*>(a.get())->object.get());
	btRigidBody &body_b = *btRigidBody::upcast(static_cast<Physics3DObjectImpl*>(b.get())->object.get());

	auto constraint = std::make_shared<Physics3DConstraintImpl>(static_cast<Physics3DWorldImpl*>(world.get()));
	constraint->constraint.reset(new btPoint2PointConstraint(body_a, body_b, btVector3(pivot_in_a.x, pivot_in_a.y, pivot_in_a.z), btVector3(pivot_in_b.x, pivot_in_b.y, pivot_in_b.z)));
	constraint->constraint->setUserConstraintPtr(constraint.get());

	static_cast<Physics3DWorldImpl*>(world.get())->dynamics_world->addConstraint(constraint->constraint.get());

	return constraint;
}

std::shared_ptr<Physics3DConstraint> Physics3DConstraint::hinge(const Physics3DWorldPtr &world, const Physics3DObjectPtr &a, const Vec3f &pivot_in_a, const Quaternionf &axis_in_a, bool use_reference_frame_a)
{
	btRigidBody &body_a = *btRigidBody::upcast(static_cast<Physics3DObjectImpl*>(a.get())->object.get());

	btTransform transform_a(btQuaternion(axis_in_a.x, axis_in_a.y, axis_in_a.z, axis_in_a.w), btVector3(pivot_in_a.x, pivot_in_a.y, pivot_in_a.z));

	auto constraint = std::make_shared<Physics3DConstraintImpl>(static_cast<Physics3DWorldImpl*>(world.get()));
	constraint->constraint.reset(new btHingeConstraint(body_a, transform_a, use_reference_frame_a));
	constraint->constraint->setUserConstraintPtr(constraint.get());

	static_cast<Physics3DWorldImpl*>(world.get())->dynamics_world->addConstraint(constraint->constraint.get());

	return constraint;
}

std::shared_ptr<Physics3DConstraint> Physics3DConstraint::hinge(const Physics3DWorldPtr &world, const Physics3DObjectPtr &a, const Physics3DObjectPtr &b, const Vec3f &pivot_in_a, const Vec3f &pivot_in_b, const Quaternionf &axis_in_a, const Quaternionf &axis_in_b, bool use_reference_frame_a)
{
	btRigidBody &body_a = *btRigidBody::upcast(static_cast<Physics3DObjectImpl*>(a.get())->object.get());
	btRigidBody &body_b = *btRigidBody::upcast(static_cast<Physics3DObjectImpl*>(b.get())->object.get());

	btTransform transform_a(btQuaternion(axis_in_a.x, axis_in_a.y, axis_in_a.z, axis_in_a.w), btVector3(pivot_in_a.x, pivot_in_a.y, pivot_in_a.z));
	btTransform transform_b(btQuaternion(axis_in_b.x, axis_in_b.y, axis_in_b.z, axis_in_b.w), btVector3(pivot_in_b.x, pivot_in_b.y, pivot_in_b.z));

	auto constraint = std::make_shared<Physics3DConstraintImpl>(static_cast<Physics3DWorldImpl*>(world.get()));
	constraint->constraint.reset(new btHingeConstraint(body_a, body_b, transform_a, transform_b, use_reference_frame_a));
	constraint->constraint->setUserConstraintPtr(constraint.get());

	static_cast<Physics3DWorldImpl*>(world.get())->dynamics_world->addConstraint(constraint->constraint.get());

	return constraint;
}

std::shared_ptr<Physics3DConstraint> Physics3DConstraint::slider(const Physics3DWorldPtr &world, const Physics3DObjectPtr &a, const Physics3DObjectPtr &b, const Vec3f &pivot_in_a, const Vec3f &pivot_in_b, const Quaternionf &axis_in_a, const Quaternionf &axis_in_b, bool use_reference_frame_a)
{
	btRigidBody &body_a = *btRigidBody::upcast(static_cast<Physics3DObjectImpl*>(a.get())->object.get());
	btRigidBody &body_b = *btRigidBody::upcast(static_cast<Physics3DObjectImpl*>(b.get())->object.get());

	btTransform transform_a(btQuaternion(axis_in_a.x, axis_in_a.y, axis_in_a.z, axis_in_a.w), btVector3(pivot_in_a.x, pivot_in_a.y, pivot_in_a.z));
	btTransform transform_b(btQuaternion(axis_in_b.x, axis_in_b.y, axis_in_b.z, axis_in_b.w), btVector3(pivot_in_b.x, pivot_in_b.y, pivot_in_b.z));

	auto constraint = std::make_shared<Physics3DConstraintImpl>(static_cast<Physics3DWorldImpl*>(world.get()));
	constraint->constraint.reset(new btSliderConstraint(body_a, body_b, transform_a, transform_b, use_reference_frame_a));
	constraint->constraint->setUserConstraintPtr(constraint.get());

	static_cast<Physics3DWorldImpl*>(world.get())->dynamics_world->addConstraint(constraint->constraint.get());

	return constraint;
}

std::shared_ptr<Physics3DConstraint> Physics3DConstraint::cone_twist(const Physics3DWorldPtr &world, const Physics3DObjectPtr &a, const Vec3f &pivot_in_a, const Quaternionf &axis_in_a)
{
	btRigidBody &body_a = *btRigidBody::upcast(static_cast<Physics3DObjectImpl*>(a.get())->object.get());

	btTransform transform_a(btQuaternion(axis_in_a.x, axis_in_a.y, axis_in_a.z, axis_in_a.w), btVector3(pivot_in_a.x, pivot_in_a.y, pivot_in_a.z));

	auto constraint = std::make_shared<Physics3DConstraintImpl>(static_cast<Physics3DWorldImpl*>(world.get()));
	constraint->constraint.reset(new btConeTwistConstraint(body_a, transform_a));
	constraint->constraint->setUserConstraintPtr(constraint.get());

	static_cast<Physics3DWorldImpl*>(world.get())->dynamics_world->addConstraint(constraint->constraint.get());

	return constraint;
}

std::shared_ptr<Physics3DConstraint> Physics3DConstraint::cone_twist(const Physics3DWorldPtr &world, const Physics3DObjectPtr &a, const Physics3DObjectPtr &b, const Vec3f &pivot_in_a, const Vec3f &pivot_in_b, const Quaternionf &axis_in_a, const Quaternionf &axis_in_b)
{
	btRigidBody &body_a = *btRigidBody::upcast(static_cast<Physics3DObjectImpl*>(a.get())->object.get());
	btRigidBody &body_b = *btRigidBody::upcast(static_cast<Physics3DObjectImpl*>(b.get())->object.get());

	btTransform transform_a(btQuaternion(axis_in_a.x, axis_in_a.y, axis_in_a.z, axis_in_a.w), btVector3(pivot_in_a.x, pivot_in_a.y, pivot_in_a.z));
	btTransform transform_b(btQuaternion(axis_in_b.x, axis_in_b.y, axis_in_b.z, axis_in_b.w), btVector3(pivot_in_b.x, pivot_in_b.y, pivot_in_b.z));

	auto constraint = std::make_shared<Physics3DConstraintImpl>(static_cast<Physics3DWorldImpl*>(world.get()));
	constraint->constraint.reset(new btConeTwistConstraint(body_a, body_b, transform_a, transform_b));
	constraint->constraint->setUserConstraintPtr(constraint.get());

	static_cast<Physics3DWorldImpl*>(world.get())->dynamics_world->addConstraint(constraint->constraint.get());

	return constraint;
}

std::shared_ptr<Physics3DConstraint> Physics3DConstraint::six_degrees_of_freedom(const Physics3DWorldPtr &world, const Physics3DObjectPtr &a, const Physics3DObjectPtr &b, const Vec3f &pivot_in_a, const Vec3f &pivot_in_b, const Quaternionf &axis_in_a, const Quaternionf &axis_in_b, bool use_reference_frame_a)
{
	btRigidBody &body_a = *btRigidBody::upcast(static_cast<Physics3DObjectImpl*>(a.get())->object.get());
	btRigidBody &body_b = *btRigidBody::upcast(static_cast<Physics3DObjectImpl*>(b.get())->object.get());

	btTransform transform_a(btQuaternion(axis_in_a.x, axis_in_a.y, axis_in_a.z, axis_in_a.w), btVector3(pivot_in_a.x, pivot_in_a.y, pivot_in_a.z));
	btTransform transform_b(btQuaternion(axis_in_b.x, axis_in_b.y, axis_in_b.z, axis_in_b.w), btVector3(pivot_in_b.x, pivot_in_b.y, pivot_in_b.z));

	auto constraint = std::make_shared<Physics3DConstraintImpl>(static_cast<Physics3DWorldImpl*>(world.get()));
	constraint->constraint.reset(new btGeneric6DofConstraint(body_a, body_b, transform_a, transform_b, use_reference_frame_a));
	constraint->constraint->setUserConstraintPtr(constraint.get());

	static_cast<Physics3DWorldImpl*>(world.get())->dynamics_world->addConstraint(constraint->constraint.get());

	return constraint;
}

/////////////////////////////////////////////////////////////////////////////

Physics3DConstraintImpl::Physics3DConstraintImpl(Physics3DWorldImpl *world)
	: world(world)
{
}

Physics3DConstraintImpl::~Physics3DConstraintImpl()
{
	if (constraint)
		world->dynamics_world->removeConstraint(constraint.get());
}

void Physics3DConstraintImpl::set_cone_twist_limit(float swing_span1, float swing_span2, float twist_span, float softness, float bias_factor, float relaxation_factor)
{
	static_cast<btConeTwistConstraint*>(constraint.get())->setLimit(swing_span1, swing_span2, twist_span, softness, bias_factor, relaxation_factor);
}

void Physics3DConstraintImpl::set_hinge_limit(float low, float high, float softness, float bias_factor, float relaxation_factor)
{
	static_cast<btHingeConstraint*>(constraint.get())->setLimit(low, high, softness, bias_factor, relaxation_factor);
}

void Physics3DConstraintImpl::set_linear_lower_limit(const Vec3f &limit)
{
	static_cast<btGeneric6DofConstraint*>(constraint.get())->setLinearLowerLimit(btVector3(limit.x, limit.y, limit.z));
}

void Physics3DConstraintImpl::set_linear_upper_limit(const Vec3f &limit)
{
	static_cast<btGeneric6DofConstraint*>(constraint.get())->setLinearUpperLimit(btVector3(limit.x, limit.y, limit.z));
}
