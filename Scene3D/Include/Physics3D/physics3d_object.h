
#pragma once

#include <memory>

class Physics3DWorld;
typedef std::shared_ptr<Physics3DWorld> Physics3DWorldPtr;
class Physics3DShape;
typedef std::shared_ptr<Physics3DShape> Physics3DShapePtr;
class Physics3DConstraint;
typedef std::shared_ptr<Physics3DConstraint> Physics3DConstraintPtr;

/*
class Physics3DRigidBodyDescription
{
public:
	Physics3DShape shape;
	uicore::Vec3f position;
	uicore::Quaternionf orientation;
	float mass;
	uicore::Vec3f local_inertia;
	float linear_damping;
	float angular_damping;
	float friction;
	float rolling_friction;
	float restitution;
	float linear_sleeping_threshold;
	float angular_sleeping_threshold;
	bool additional_damping;
	float additional_damping_factor;
	float additional_linear_damping_threshold_sqr;
	float additional_angular_damping_threshold_sqr;
	float additional_angular_damping_factor;
};
*/

class Physics3DObject
{
public:
	static std::shared_ptr<Physics3DObject> collision_body(const Physics3DWorldPtr &world, const Physics3DShapePtr &shape, const uicore::Vec3f &position = uicore::Vec3f(0.0f), const uicore::Quaternionf &orientation = uicore::Quaternionf());
	static std::shared_ptr<Physics3DObject> ghost_body(const Physics3DWorldPtr &world, const Physics3DShapePtr &shape, const uicore::Vec3f &position = uicore::Vec3f(0.0f), const uicore::Quaternionf &orientation = uicore::Quaternionf());
	static std::shared_ptr<Physics3DObject> rigid_body(const Physics3DWorldPtr &world, const Physics3DShapePtr &shape, float mass = 0.0f, const uicore::Vec3f &position = uicore::Vec3f(0.0f), const uicore::Quaternionf &orientation = uicore::Quaternionf(), const uicore::Vec3f &local_inertia = uicore::Vec3f());
	//static std::shared_ptr<Physics3DObject> rigid_body(const Physics3DWorldPtr &world, const Physics3DRigidBodyDescription &desc);
	//static std::shared_ptr<Physics3DObject> soft_body(const Physics3DWorldPtr &world, const Physics3DShapePtr &shape, const uicore::Vec3f &position = uicore::Vec3f(0.0f), const uicore::Quaternionf &orientation = uicore::Quaternionf());

	virtual uicore::Vec3f position() const = 0;
	virtual uicore::Quaternionf orientation() const = 0;

	virtual bool static_object() const = 0;
	virtual bool kinematic_object() const = 0;
	virtual bool character_object() const = 0;
	virtual bool debug_drawn() const = 0;

	virtual void set_position(const uicore::Vec3f &position) = 0;
	virtual void set_orientation(const uicore::Quaternionf &orientation) = 0;
	virtual void set_transform(const uicore::Vec3f &position, const uicore::Quaternionf &orientation) = 0;

	virtual void set_static_object(bool enable = true) = 0;
	virtual void set_kinematic_object(bool enable = true) = 0;
	virtual void set_character_object(bool enable = true) = 0;
	virtual void set_debug_drawn(bool enable = true) = 0;

	template<typename T>
	std::shared_ptr<T> data() { return std::shared_ptr<T>(); } // To do: remove this function and fix code depending on it

	// Rigid body functionality:

	virtual void set_mass(float mass, const uicore::Vec3f &local_inertia) = 0;
	virtual void set_sleeping_thresholds(float linear, float angular) = 0;

	virtual void set_ccd_swept_sphere_radius(float radius) = 0;
	virtual void set_ccd_motion_threshold(float motion_threshold) = 0;

	virtual void activate(bool force_activation = false) = 0;

	virtual void apply_central_force(const uicore::Vec3f &force) = 0;
	virtual void apply_torque(const uicore::Vec3f &torque) = 0;
	virtual void apply_force(const uicore::Vec3f &force, const uicore::Vec3f &relative_pos) = 0;
	virtual void apply_central_impulse(const uicore::Vec3f &force) = 0;
	virtual void apply_torque_impulse(const uicore::Vec3f &torque) = 0;
	virtual void apply_impulse(const uicore::Vec3f &impulse, const uicore::Vec3f &relative_pos) = 0;
	virtual void clear_forces() = 0;

	virtual void add_constraint(const Physics3DConstraintPtr &constraint) = 0;
	virtual void remove_constraint(const Physics3DConstraintPtr &constraint) = 0;
};

typedef std::shared_ptr<Physics3DObject> Physics3DObjectPtr;
