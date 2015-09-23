
#pragma once

namespace uicore
{
	class ModelDescAttachmentPoint
	{
	public:
		std::string name;
		Vec3f position;
		Quaternionf orientation;
		std::string bone_name;
		std::string test_model;
		float test_scale = 1.0f;
	};
}
