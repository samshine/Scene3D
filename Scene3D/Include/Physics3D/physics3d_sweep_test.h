
#pragma once

#include <memory>

class Physics3DWorld;
typedef std::shared_ptr<Physics3DWorld> Physics3DWorldPtr;
class Physics3DShape;
class Physics3DObject;
class Physics3DSweepTestImpl;

class Physics3DSweepTest
{
public:
	Physics3DSweepTest();
	Physics3DSweepTest(const Physics3DWorldPtr &world);

	bool is_null() const;

	bool test_any_hit(const Physics3DShape &shape, const uicore::Vec3f &from_pos, const uicore::Quaternionf &from_orientation, const uicore::Vec3f &to_pos, const uicore::Quaternionf &to_orientation, float allowed_ccd_penetration = 0.0f);
	bool test_first_hit(const Physics3DShape &shape, const uicore::Vec3f &from_pos, const uicore::Quaternionf &from_orientation, const uicore::Vec3f &to_pos, const uicore::Quaternionf &to_orientation, float allowed_ccd_penetration = 0.0f);
	bool test_all_hits(const Physics3DShape &shape, const uicore::Vec3f &from_pos, const uicore::Quaternionf &from_orientation, const uicore::Vec3f &to_pos, const uicore::Quaternionf &to_orientation, float allowed_ccd_penetration = 0.0f);

	int get_hit_count() const;
	float get_hit_fraction(int index) const;
	uicore::Vec3f get_hit_position(int index) const;
	uicore::Vec3f get_hit_normal(int index) const;
	Physics3DObject get_hit_object(int index) const;

private:
	std::shared_ptr<Physics3DSweepTestImpl> impl;
};
