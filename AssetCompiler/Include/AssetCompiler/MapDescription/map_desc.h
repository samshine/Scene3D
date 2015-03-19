
#pragma once

#include <vector>

namespace clan
{
	class MapDesc
	{
	public:
		MapDesc();

		std::string fbx_filename;

		static MapDesc load(const std::string &filename);
		void save(const std::string &filename);
	};
}
