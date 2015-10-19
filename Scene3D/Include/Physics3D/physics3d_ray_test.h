
#pragma once

#include <memory>

class Physics3DWorld;
typedef std::shared_ptr<Physics3DWorld> Physics3DWorldPtr;
class Physics3DObject;
class Physics3DRayTestImpl;

class Physics3DRayTest
{
public:
	Physics3DRayTest();
	Physics3DRayTest(const Physics3DWorldPtr &world);

	bool is_null() const;

	bool test(const uicore::Vec3f &start, const uicore::Vec3f &end);

	bool has_hit() const;
	float get_hit_fraction() const;
	uicore::Vec3f get_hit_position() const;
	uicore::Vec3f get_hit_normal() const;
	Physics3DObject get_hit_object() const;

private:
	std::shared_ptr<Physics3DRayTestImpl> impl;
};
