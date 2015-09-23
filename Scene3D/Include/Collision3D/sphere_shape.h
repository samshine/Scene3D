
#pragma once

namespace uicore
{
	class SphereShape
	{
	public:
		SphereShape() { }
		SphereShape(const Vec3f &center, float radius) : center(center), radius(radius) { }

		Vec3f center;
		float radius = 0.0f;
	};
}
