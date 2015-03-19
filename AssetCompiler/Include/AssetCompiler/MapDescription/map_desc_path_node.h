
#pragma once

#include <vector>

namespace clan
{
	class MapDescPathNode
	{
	public:
		Vec3f position;
		std::vector<int> connections;
	};
}
