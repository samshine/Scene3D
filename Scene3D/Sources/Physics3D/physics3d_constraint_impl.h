
#pragma once

#include "Physics3D/Bullet/btBulletDynamicsCommon.h"
#include <vector>
#include <memory>

class Physics3DConstraint_Impl
{
public:
	Physics3DConstraint_Impl(Physics3DWorld_Impl *world);
	~Physics3DConstraint_Impl();

	Physics3DWorld_Impl *world;
	std::unique_ptr<btTypedConstraint> constraint;
};
