
#pragma once

#include <string>
#include <memory>

class Physics3DWorld;
typedef std::shared_ptr<Physics3DWorld> Physics3DWorldPtr;
class Physics3DObject;
typedef std::shared_ptr<Physics3DObject> Physics3DObjectPtr;

class Physics3DConstraint
{
public:
	static std::shared_ptr<Physics3DConstraint> point_to_point(const Physics3DWorldPtr &world, const Physics3DObjectPtr &body_a, const uicore::Vec3f &pivot_in_a);
	static std::shared_ptr<Physics3DConstraint> point_to_point(const Physics3DWorldPtr &world, const Physics3DObjectPtr &body_a, const Physics3DObjectPtr &body_b, const uicore::Vec3f &pivot_in_a, const uicore::Vec3f &pivot_in_b);
	static std::shared_ptr<Physics3DConstraint> hinge(const Physics3DWorldPtr &world, const Physics3DObjectPtr &body_a, const uicore::Vec3f &pivot_in_a, const uicore::Quaternionf &axis_in_a, bool use_reference_frame_a = false);
	static std::shared_ptr<Physics3DConstraint> hinge(const Physics3DWorldPtr &world, const Physics3DObjectPtr &body_a, const Physics3DObjectPtr &body_b, const uicore::Vec3f &pivot_in_a, const uicore::Vec3f &pivot_in_b, const uicore::Quaternionf &axis_in_a, const uicore::Quaternionf &axis_in_b, bool use_reference_frame_a = false);
	static std::shared_ptr<Physics3DConstraint> slider(const Physics3DWorldPtr &world, const Physics3DObjectPtr &body_a, const Physics3DObjectPtr &body_b, const uicore::Vec3f &pivot_in_a, const uicore::Vec3f &pivot_in_b, const uicore::Quaternionf &axis_in_a, const uicore::Quaternionf &axis_in_b, bool use_reference_frame_a = false);
	static std::shared_ptr<Physics3DConstraint> cone_twist(const Physics3DWorldPtr &world, const Physics3DObjectPtr &body_a, const uicore::Vec3f &pivot_in_a, const uicore::Quaternionf &axis_in_a);
	static std::shared_ptr<Physics3DConstraint> cone_twist(const Physics3DWorldPtr &world, const Physics3DObjectPtr &body_a, const Physics3DObjectPtr &body_b, const uicore::Vec3f &pivot_in_a, const uicore::Vec3f &pivot_in_b, const uicore::Quaternionf &axis_in_a, const uicore::Quaternionf &axis_in_b);
	static std::shared_ptr<Physics3DConstraint> six_degrees_of_freedom(const Physics3DWorldPtr &world, const Physics3DObjectPtr &body_a, const Physics3DObjectPtr &body_b, const uicore::Vec3f &pivot_in_a, const uicore::Vec3f &pivot_in_b, const uicore::Quaternionf &axis_in_a, const uicore::Quaternionf &axis_in_b, bool use_reference_frame_a = false);

	// Six degrees of freedom only:
	virtual void set_linear_lower_limit(const uicore::Vec3f &limit) = 0;
	virtual void set_linear_upper_limit(const uicore::Vec3f &limit) = 0;
};

typedef std::shared_ptr<Physics3DConstraint> Physics3DConstraintPtr;
