
#pragma once

#include "Physics3D/Bullet/btBulletDynamicsCommon.h"

class Physics3DObject_Impl;
class Physics3DWorld_Impl;

class Physics3DContactPairTest_Impl
{
public:
	Physics3DContactPairTest_Impl(Physics3DWorld_Impl *world);
	~Physics3DContactPairTest_Impl();

	Physics3DWorld_Impl *world;
};
