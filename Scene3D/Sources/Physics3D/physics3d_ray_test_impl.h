
#pragma once

#include "Physics3D/Bullet/btBulletDynamicsCommon.h"

class Physics3DObject_Impl;
class Physics3DWorld_Impl;

class Physics3DRayTest_Impl
{
public:
	Physics3DRayTest_Impl(Physics3DWorld_Impl *world);
	~Physics3DRayTest_Impl();

	Physics3DWorld_Impl *world;

	uicore::Vec3f start, end;
	bool has_hit;
	float hit_fraction;
	uicore::Vec3f hit_normal;
	Physics3DObject_Impl *hit_object;
};
