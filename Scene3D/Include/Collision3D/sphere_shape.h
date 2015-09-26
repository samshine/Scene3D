
#pragma once

class SphereShape
{
public:
	SphereShape() { }
	SphereShape(const uicore::Vec3f &center, float radius) : center(center), radius(radius) { }

	uicore::Vec3f center;
	float radius = 0.0f;
};
