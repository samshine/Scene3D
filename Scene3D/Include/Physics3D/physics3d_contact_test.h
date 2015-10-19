
#pragma once

#include <memory>

class Physics3DWorld;
typedef std::shared_ptr<Physics3DWorld> Physics3DWorldPtr;
class Physics3DObject;
class Physics3DShape;
class Physics3DContactTestImpl;

class Physics3DContactTest
{
public:
	Physics3DContactTest();
	Physics3DContactTest(const Physics3DWorldPtr &world);

	bool is_null() const;

	bool test(const Physics3DObject &object);
	bool test(const Physics3DShape &shape, const uicore::Vec3f &position, const uicore::Quaternionf &orientation);

	int get_hit_count() const;
	Physics3DObject get_hit_object(int index) const;
	uicore::Vec3f get_hit_position(int index) const;
	uicore::Vec3f get_hit_normal(int index) const;
	float get_hit_distance(int index) const;

private:
	std::shared_ptr<Physics3DContactTestImpl> impl;
};
