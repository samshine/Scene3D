
#pragma once

class GameSoundCache : public uicore::SoundCache
{
public:
	uicore::Resource<uicore::SoundBuffer> get_sound(const std::string &id) override;

private:
	std::map<std::string, uicore::Resource<uicore::SoundBuffer>> sounds;
};

inline uicore::Resource<uicore::SoundBuffer> GameSoundCache::get_sound(const std::string &id)
{
	auto it = sounds.find(id);
	if (it != sounds.end())
		return it->second;

	sounds[id] = uicore::Resource<uicore::SoundBuffer>(uicore::SoundBuffer(uicore::PathHelp::combine("Resources", id)));
	return sounds[id];
}
