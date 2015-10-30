
#pragma once

#include <string>
#include <memory>

class Physics3DWorld;
typedef std::shared_ptr<Physics3DWorld> Physics3DWorldPtr;
class Physics3DObject;
typedef std::shared_ptr<Physics3DObject> Physics3DObjectPtr;

class Physics3DConstraintObject;

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

	// Cone twist only:
	virtual void set_cone_twist_limit(float swing_span1, float swing_span2, float twist_span, float softness = 1.0f, float bias_factor = 0.3f, float relaxation_factor = 1.0f) = 0;

	// Hinge only:
	virtual void set_hinge_limit(float low, float high, float softness = 0.9f, float bias_factor = 0.3f, float relaxation_factor = 1.0f) = 0;

	// Six degrees of freedom only:
	virtual void set_linear_lower_limit(const uicore::Vec3f &limit) = 0;
	virtual void set_linear_upper_limit(const uicore::Vec3f &limit) = 0;
};
