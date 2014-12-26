
#pragma once

#include <ClanLib/core.h>

namespace clan
{
	class FBXMaterial
	{
	public:
		std::string mesh_material;
		bool two_sided = false;
		bool alpha_test = false;
		bool transparent = false;
	};
}
