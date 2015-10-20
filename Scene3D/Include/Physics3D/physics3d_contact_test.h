
#pragma once

#include <memory>

class Physics3DWorld;
typedef std::shared_ptr<Physics3DWorld> Physics3DWorldPtr;
class Physics3DObject;
class Physics3DShape;
typedef std::shared_ptr<Physics3DShape> Physics3DShapePtr;

class Physics3DContactTest
{
public:
	static std::shared_ptr<Physics3DContactTest> create(const Physics3DWorldPtr &world);

	virtual bool test(const Physics3DObject &object) = 0;
	virtual bool test(const Physics3DShapePtr &shape, const uicore::Vec3f &position, const uicore::Quaternionf &orientation) = 0;

	virtual int hit_count() const = 0;
	virtual Physics3DObject hit_object(int index) const = 0;
	virtual uicore::Vec3f hit_position(int index) const = 0;
	virtual uicore::Vec3f hit_normal(int index) const = 0;
	virtual float hit_distance(int index) const = 0;
};

typedef std::shared_ptr<Physics3DContactTest> Physics3DContactTestPtr;
