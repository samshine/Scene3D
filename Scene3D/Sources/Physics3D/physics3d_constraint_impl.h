
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

	void set_linear_lower_limit(const uicore::Vec3f &limit) override;
	void set_linear_upper_limit(const uicore::Vec3f &limit) override;

	Physics3DWorldImpl *world = nullptr;
	std::unique_ptr<btTypedConstraint> constraint;
};
