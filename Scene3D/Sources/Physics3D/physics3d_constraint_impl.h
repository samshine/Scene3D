
#pragma once

#include "Physics3D/physics3d_constraint.h"
#include "Physics3D/Bullet/btBulletDynamicsCommon.h"
#include <vector>
#include <memory>

class Physics3DWorldImpl;

class Physics3DConstraintImpl : public Physics3DConstraint
{
public:
	Physics3DConstraintImpl(Physics3DWorldImpl *world);
	~Physics3DConstraintImpl();

	void set_cone_twist_limit(float swing_span1, float swing_span2, float twist_span, float softness, float bias_factor, float relaxation_factor) override;
	void set_hinge_limit(float low, float high, float softness, float bias_factor, float relaxation_factor) override;

	void set_linear_lower_limit(const uicore::Vec3f &limit) override;
	void set_linear_upper_limit(const uicore::Vec3f &limit) override;

	Physics3DWorldImpl *world = nullptr;
	std::unique_ptr<btTypedConstraint> constraint;
};
