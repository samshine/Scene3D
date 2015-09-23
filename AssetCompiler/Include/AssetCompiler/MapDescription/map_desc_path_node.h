
#pragma once

#include <vector>

namespace uicore
{
	class MapDescPathNode
	{
	public:
		Vec3f position;
		std::vector<int> connections;
	};
}
