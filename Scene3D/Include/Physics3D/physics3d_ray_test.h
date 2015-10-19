
#pragma once

#include <memory>

class Physics3DWorld;
typedef std::shared_ptr<Physics3DWorld> Physics3DWorldPtr;
class Physics3DObject;

class Physics3DRayTest
{
public:
	static std::shared_ptr<Physics3DRayTest> create(const Physics3DWorldPtr &world);

	virtual bool test(const uicore::Vec3f &start, const uicore::Vec3f &end) = 0;

	virtual bool hit() const = 0;
	virtual float hit_fraction() const = 0;
	virtual uicore::Vec3f hit_position() const = 0;
	virtual uicore::Vec3f hit_normal() const = 0;
	virtual Physics3DObject hit_object() const = 0;
};

typedef std::shared_ptr<Physics3DRayTest> Physics3DRayTestPtr;
