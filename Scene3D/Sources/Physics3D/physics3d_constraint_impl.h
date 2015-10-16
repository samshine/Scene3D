
#pragma once

#include "Physics3D/Bullet/btBulletDynamicsCommon.h"
#include <vector>
#include <memory>

class Physics3DConstraintImpl
{
public:
	Physics3DConstraintImpl(Physics3DWorldImpl *world);
	~Physics3DConstraintImpl();

	Physics3DWorldImpl *world;
	std::unique_ptr<btTypedConstraint> constraint;
};
