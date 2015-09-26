
#pragma once

#include "Physics3D/physics3d_shape.h"
#include "Physics3D/Bullet/btBulletDynamicsCommon.h"

class Physics3DWorld_Impl;

class Physics3DObject_Impl : public std::enable_shared_from_this<Physics3DObject_Impl>
{
public:
	Physics3DObject_Impl(Physics3DWorld_Impl *world);
	~Physics3DObject_Impl();

	Physics3DWorld_Impl *world;
	Physics3DShape shape;
	std::unique_ptr<btCollisionObject> object;
};
