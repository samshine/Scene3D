
#pragma once

namespace clan
{
	class ModelDescMaterial
	{
	public:
		std::string mesh_material;
		bool two_sided = false;
		bool alpha_test = false;
		bool transparent = false;
		bool no_collision = false;
		bool no_render = false;
	};
}
