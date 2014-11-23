
#pragma once

#include <ClanLib/core.h>

namespace clan
{
	class FBXAttachmentPoint
	{
	public:
		std::string name;
		clan::Vec3f position;
		clan::Quaternionf orientation;
		std::string bone_name;
	};
}
