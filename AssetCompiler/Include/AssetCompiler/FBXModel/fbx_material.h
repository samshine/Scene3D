
#pragma once

#include <ClanLib/core.h>

namespace clan
{
	class FBXMaterial
	{
	public:
		std::string name;
		bool two_sided = false;
		bool alpha_test = false;
		std::string mesh_material;
	};
}
