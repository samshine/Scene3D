
#pragma once

#include "Physics3D/physics3d_shape.h"
#include "Physics3D/Bullet/btBulletDynamicsCommon.h"

class Physics3DWorldImpl;

class Physics3DObjectImpl : public std::enable_shared_from_this<Physics3DObjectImpl>
{
public:
	Physics3DObjectImpl(Physics3DWorldImpl *world);
	~Physics3DObjectImpl();

	Physics3DWorldImpl *world;
	Physics3DShapePtr shape;
	std::unique_ptr<btCollisionObject> object;
};
