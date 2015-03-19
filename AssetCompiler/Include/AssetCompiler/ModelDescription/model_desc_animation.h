
#pragma once

#include <string>

namespace clan
{
	class ModelDescAnimation
	{
	public:
		std::string name;
		int start_frame = 0;
		int end_frame = 0;
		float play_speed = 1.0f;
		float move_speed = 1.0f;
		bool loop = true;
		int rarity = 65535;
	};
}
