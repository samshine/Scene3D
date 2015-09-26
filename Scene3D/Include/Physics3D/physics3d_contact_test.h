
#pragma once

#include <memory>

class Physics3DWorld;
class Physics3DObject;
class Physics3DShape;
class Physics3DContactTest_Impl;

class Physics3DContactTest
{
public:
	Physics3DContactTest();
	Physics3DContactTest(Physics3DWorld &world);

	bool is_null() const;

	bool test(const Physics3DObject &object);
	bool test(const Physics3DShape &shape, const uicore::Vec3f &position, const uicore::Quaternionf &orientation);

	int get_hit_count() const;
	Physics3DObject get_hit_object(int index) const;
	uicore::Vec3f get_hit_position(int index) const;
	uicore::Vec3f get_hit_normal(int index) const;
	float get_hit_distance(int index) const;

private:
	std::shared_ptr<Physics3DContactTest_Impl> impl;
};
