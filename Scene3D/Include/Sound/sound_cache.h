
#pragma once

#include "soundbuffer.h"

namespace uicore
{
	class SoundCache
	{
	public:
		SoundBuffer get(const std::string &id)
		{
			auto it = sounds.find(id);
			if (it != sounds.end())
				return it->second;

			sounds[id] = SoundBuffer(SoundBuffer(PathHelp::combine("Resources", id)));
			return sounds[id];
		}

	private:
		std::map<std::string, SoundBuffer> sounds;
	};
}
