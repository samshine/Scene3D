
#pragma once

#include <string>
#include <memory>

class Physics3DWorld;
class Physics3DObject;
class Physics3DConstraint_Impl;

class Physics3DConstraint
{
public:
	Physics3DConstraint();

	bool is_null() const;

	static Physics3DConstraint point_to_point(Physics3DWorld &world, const Physics3DObject &body_a, const uicore::Vec3f &pivot_in_a);
	static Physics3DConstraint point_to_point(Physics3DWorld &world, const Physics3DObject &body_a, const Physics3DObject &body_b, const uicore::Vec3f &pivot_in_a, const uicore::Vec3f &pivot_in_b);
	static Physics3DConstraint hinge(Physics3DWorld &world, const Physics3DObject &body_a, const uicore::Vec3f &pivot_in_a, const uicore::Quaternionf &axis_in_a, bool use_reference_frame_a = false);
	static Physics3DConstraint hinge(Physics3DWorld &world, const Physics3DObject &body_a, const Physics3DObject &body_b, const uicore::Vec3f &pivot_in_a, const uicore::Vec3f &pivot_in_b, const uicore::Quaternionf &axis_in_a, const uicore::Quaternionf &axis_in_b, bool use_reference_frame_a = false);
	static Physics3DConstraint slider(Physics3DWorld &world, const Physics3DObject &body_a, const Physics3DObject &body_b, const uicore::Vec3f &pivot_in_a, const uicore::Vec3f &pivot_in_b, const uicore::Quaternionf &axis_in_a, const uicore::Quaternionf &axis_in_b, bool use_reference_frame_a = false);
	static Physics3DConstraint cone_twist(Physics3DWorld &world, const Physics3DObject &body_a, const uicore::Vec3f &pivot_in_a, const uicore::Quaternionf &axis_in_a);
	static Physics3DConstraint cone_twist(Physics3DWorld &world, const Physics3DObject &body_a, const Physics3DObject &body_b, const uicore::Vec3f &pivot_in_a, const uicore::Vec3f &pivot_in_b, const uicore::Quaternionf &axis_in_a, const uicore::Quaternionf &axis_in_b);
	static Physics3DConstraint six_degrees_of_freedom(Physics3DWorld &world, const Physics3DObject &body_a, const Physics3DObject &body_b, const uicore::Vec3f &pivot_in_a, const uicore::Vec3f &pivot_in_b, const uicore::Quaternionf &axis_in_a, const uicore::Quaternionf &axis_in_b, bool use_reference_frame_a = false);

	// Six degrees of freedom only:
	void set_linear_lower_limit(const uicore::Vec3f &limit);
	void set_linear_upper_limit(const uicore::Vec3f &limit);

private:
	std::shared_ptr<Physics3DConstraint_Impl> impl;

	friend class Physics3DObject;
};
