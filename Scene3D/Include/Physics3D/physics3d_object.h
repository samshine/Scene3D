/*
**  ClanLib SDK
**  Copyright (c) 1997-2013 The ClanLib Team
**
**  This software is provided 'as-is', without any express or implied
**  warranty.  In no event will the authors be held liable for any damages
**  arising from the use of this software.
**
**  Permission is granted to anyone to use this software for any purpose,
**  including commercial applications, and to alter it and redistribute it
**  freely, subject to the following restrictions:
**
**  1. The origin of this software must not be misrepresented; you must not
**     claim that you wrote the original software. If you use this software
**     in a product, an acknowledgment in the product documentation would be
**     appreciated but is not required.
**  2. Altered source versions must be plainly marked as such, and must not be
**     misrepresented as being the original software.
**  3. This notice may not be removed or altered from any source distribution.
**
**  Note: Some of the libraries ClanLib may link to may have additional
**  requirements or restrictions.
**
**  File Author(s):
**
**    Magnus Norddahl
*/


#pragma once

#include <memory>
#include "../Scene3D/Resources/userdata.h"

namespace uicore
{
/// \addtogroup clanPhysics3D_World clanPhysics3D World
/// \{

class Physics3DWorld;
class Physics3DShape;
class Physics3DConstraint;
class Physics3DObject_Impl;
/*
class Physics3DRigidBodyDescription
{
public:
	Physics3DShape shape;
	Vec3f position;
	Quaternionf orientation;
	float mass;
	Vec3f local_inertia;
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
	static Physics3DObject collision_body(Physics3DWorld &world, const Physics3DShape &shape, const Vec3f &position = Vec3f(0.0f), const Quaternionf &orientation = Quaternionf());
	static Physics3DObject ghost_body(Physics3DWorld &world, const Physics3DShape &shape, const Vec3f &position = Vec3f(0.0f), const Quaternionf &orientation = Quaternionf());
	static Physics3DObject rigid_body(Physics3DWorld &world, const Physics3DShape &shape, float mass = 0.0f, const Vec3f &position = Vec3f(0.0f), const Quaternionf &orientation = Quaternionf(), const Vec3f &local_inertia = Vec3f());
	//static Physics3DObject rigid_body(Physics3DWorld &world, const Physics3DRigidBodyDescription &desc);
	//static Physics3DObject soft_body(Physics3DWorld &world, const Physics3DShape &shape, const Vec3f &position = Vec3f(0.0f), const Quaternionf &orientation = Quaternionf());

	Physics3DObject();
	Physics3DObject(std::shared_ptr<Physics3DObject_Impl> impl);
	bool is_null() const { return !impl; }

	Vec3f get_position() const;
	Quaternionf get_orientation() const;

	bool is_static() const;
	bool is_kinematic() const;
	bool is_character_object() const;
	bool is_debug_drawn() const;

	void set_position(const Vec3f &position);
	void set_orientation(const Quaternionf &orientation);
	void set_transform(const Vec3f &position, const Quaternionf &orientation);

	void set_static(bool enable);
	void set_kinematic(bool enable);
	void set_character_object(bool enable);
	void set_debug_drawn(bool enable);

	template<typename T>
	void set_data(const std::shared_ptr<T> &data)
	{
		get_userdata_owner()->set_data<T>(data);
	}

	template<typename T>
	std::shared_ptr<T> get_data()
	{
		return get_userdata_owner()->get_data<T>();
	}

	// Rigid body functionality:

	void set_mass(float mass, const Vec3f &local_inertia);
	void set_sleeping_thresholds(float linear, float angular);

	void set_ccd_swept_sphere_radius(float radius);
	void set_ccd_motion_threshold(float motion_threshold);

	void activate(bool force_activation = false);

	void apply_central_force(const Vec3f &force);
	void apply_torque(const Vec3f &torque);
	void apply_force(const Vec3f &force, const Vec3f &relative_pos);
	void apply_central_impulse(const Vec3f &force);
	void apply_torque_impulse(const Vec3f &torque);
	void apply_impulse(const Vec3f &impulse, const Vec3f &relative_pos);
	void clear_forces();

	void add_constraint(const Physics3DConstraint &constraint);
	void remove_constraint(const Physics3DConstraint &constraint);

private:
	UserDataOwner *get_userdata_owner();

	std::shared_ptr<Physics3DObject_Impl> impl;

	friend class Physics3DContactTest;
	friend class Physics3DConstraint;
};

}

/// \}
