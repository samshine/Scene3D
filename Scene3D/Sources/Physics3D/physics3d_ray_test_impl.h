
#pragma once

#include "Physics3D/Bullet/btBulletDynamicsCommon.h"

class Physics3DObjectImpl;
class Physics3DWorldImpl;

class Physics3DRayTestImpl
{
public:
	Physics3DRayTestImpl(Physics3DWorldImpl *world);
	~Physics3DRayTestImpl();

	Physics3DWorldImpl *world;

	uicore::Vec3f start, end;
	bool has_hit;
	float hit_fraction;
	uicore::Vec3f hit_normal;
	Physics3DObjectImpl *hit_object;
};
