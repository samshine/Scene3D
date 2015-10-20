
#pragma once

#include "Physics3D/physics3d_contact_pair_test.h"
#include "Physics3D/Bullet/btBulletDynamicsCommon.h"

class Physics3DObjectImpl;
class Physics3DWorldImpl;

class Physics3DContactPairTestImpl : public Physics3DContactPairTest
{
public:
	Physics3DContactPairTestImpl(Physics3DWorldImpl *world);
	~Physics3DContactPairTestImpl();

	bool test(const Physics3DObjectPtr &object_a, const Physics3DObjectPtr &object_b) override;

private:
	Physics3DWorldImpl *world;
};
