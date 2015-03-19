
#pragma once

namespace clan
{
	class MapDescObject
	{
	public:
		std::string id;
		Vec3f position;
		float dir = 0.0f;
		float up = 0.0f;
		float tilt = 0.0f;
		float scale = 1.0f;
		std::string model_desc_filename;
	};
}
