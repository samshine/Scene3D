
#pragma once

#include <memory>

class Physics3DWorld;
class Physics3DShape;
class Physics3DConstraint;
class Physics3DObject_Impl;

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
	static Physics3DObject collision_body(Physics3DWorld &world, const Physics3DShape &shape, const uicore::Vec3f &position = uicore::Vec3f(0.0f), const uicore::Quaternionf &orientation = uicore::Quaternionf());
	static Physics3DObject ghost_body(Physics3DWorld &world, const Physics3DShape &shape, const uicore::Vec3f &position = uicore::Vec3f(0.0f), const uicore::Quaternionf &orientation = uicore::Quaternionf());
	static Physics3DObject rigid_body(Physics3DWorld &world, const Physics3DShape &shape, float mass = 0.0f, const uicore::Vec3f &position = uicore::Vec3f(0.0f), const uicore::Quaternionf &orientation = uicore::Quaternionf(), const uicore::Vec3f &local_inertia = uicore::Vec3f());
	//static Physics3DObject rigid_body(Physics3DWorld &world, const Physics3DRigidBodyDescription &desc);
	//static Physics3DObject soft_body(Physics3DWorld &world, const Physics3DShape &shape, const uicore::Vec3f &position = uicore::Vec3f(0.0f), const uicore::Quaternionf &orientation = uicore::Quaternionf());

	Physics3DObject();
	Physics3DObject(std::shared_ptr<Physics3DObject_Impl> impl);
	bool is_null() const { return !impl; }

	uicore::Vec3f get_position() const;
	uicore::Quaternionf get_orientation() const;

	bool is_static() const;
	bool is_kinematic() const;
	bool is_character_object() const;
	bool is_debug_drawn() const;

	void set_position(const uicore::Vec3f &position);
	void set_orientation(const uicore::Quaternionf &orientation);
	void set_transform(const uicore::Vec3f &position, const uicore::Quaternionf &orientation);

	void set_static(bool enable);
	void set_kinematic(bool enable);
	void set_character_object(bool enable);
	void set_debug_drawn(bool enable);

	template<typename T>
	std::shared_ptr<T> get_data() { return std::shared_ptr<T>(); } // To do: remove this function and fix code depending on it

	// Rigid body functionality:

	void set_mass(float mass, const uicore::Vec3f &local_inertia);
	void set_sleeping_thresholds(float linear, float angular);

	void set_ccd_swept_sphere_radius(float radius);
	void set_ccd_motion_threshold(float motion_threshold);

	void activate(bool force_activation = false);

	void apply_central_force(const uicore::Vec3f &force);
	void apply_torque(const uicore::Vec3f &torque);
	void apply_force(const uicore::Vec3f &force, const uicore::Vec3f &relative_pos);
	void apply_central_impulse(const uicore::Vec3f &force);
	void apply_torque_impulse(const uicore::Vec3f &torque);
	void apply_impulse(const uicore::Vec3f &impulse, const uicore::Vec3f &relative_pos);
	void clear_forces();

	void add_constraint(const Physics3DConstraint &constraint);
	void remove_constraint(const Physics3DConstraint &constraint);

private:
	std::shared_ptr<Physics3DObject_Impl> impl;

	friend class Physics3DContactTest;
	friend class Physics3DConstraint;
};
