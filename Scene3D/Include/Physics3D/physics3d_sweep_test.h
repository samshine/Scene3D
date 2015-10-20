
#pragma once

#include <memory>

class Physics3DWorld;
typedef std::shared_ptr<Physics3DWorld> Physics3DWorldPtr;
class Physics3DShape;
typedef std::shared_ptr<Physics3DShape> Physics3DShapePtr;
class Physics3DObject;
typedef std::shared_ptr<Physics3DObject> Physics3DObjectPtr;

class Physics3DSweepTest
{
public:
	static std::shared_ptr<Physics3DSweepTest> create(const Physics3DWorldPtr &world);

	virtual bool test_any_hit(const Physics3DShapePtr &shape, const uicore::Vec3f &from_pos, const uicore::Quaternionf &from_orientation, const uicore::Vec3f &to_pos, const uicore::Quaternionf &to_orientation, float allowed_ccd_penetration = 0.0f) = 0;
	virtual bool test_first_hit(const Physics3DShapePtr &shape, const uicore::Vec3f &from_pos, const uicore::Quaternionf &from_orientation, const uicore::Vec3f &to_pos, const uicore::Quaternionf &to_orientation, float allowed_ccd_penetration = 0.0f) = 0;
	virtual bool test_all_hits(const Physics3DShapePtr &shape, const uicore::Vec3f &from_pos, const uicore::Quaternionf &from_orientation, const uicore::Vec3f &to_pos, const uicore::Quaternionf &to_orientation, float allowed_ccd_penetration = 0.0f) = 0;

	virtual int hit_count() const = 0;
	virtual float hit_fraction(int index) const = 0;
	virtual uicore::Vec3f hit_position(int index) const = 0;
	virtual uicore::Vec3f hit_normal(int index) const = 0;
	virtual Physics3DObjectPtr hit_object(int index) const = 0;
};

typedef std::shared_ptr<Physics3DSweepTest> Physics3DSweepTestPtr;
