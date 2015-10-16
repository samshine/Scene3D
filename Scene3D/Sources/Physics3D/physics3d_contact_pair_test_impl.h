
#pragma once

#include "Physics3D/Bullet/btBulletDynamicsCommon.h"

class Physics3DObjectImpl;
class Physics3DWorldImpl;

class Physics3DContactPairTestImpl
{
public:
	Physics3DContactPairTestImpl(Physics3DWorldImpl *world);
	~Physics3DContactPairTestImpl();

	Physics3DWorldImpl *world;
};
