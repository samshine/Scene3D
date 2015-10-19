
#pragma once

#include "Physics3D/physics3d_ray_test.h"
#include "Physics3D/Bullet/btBulletDynamicsCommon.h"

class Physics3DObjectImpl;
class Physics3DWorldImpl;

class Physics3DRayTestImpl : public Physics3DRayTest
{
public:
	Physics3DRayTestImpl(Physics3DWorldImpl *world);
	~Physics3DRayTestImpl();

	bool test(const uicore::Vec3f &start, const uicore::Vec3f &end) override;

	bool hit() const override;
	float hit_fraction() const override;
	uicore::Vec3f hit_position() const override;
	uicore::Vec3f hit_normal() const override;
	Physics3DObject hit_object() const override;

private:
	Physics3DWorldImpl *world = nullptr;

	uicore::Vec3f start, end;
	bool _hit = false;
	float _hit_fraction = 1.0f;
	uicore::Vec3f _hit_normal;
	Physics3DObjectImpl *_hit_object = nullptr;
};
