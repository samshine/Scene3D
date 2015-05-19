
#pragma once

class SoundCacheProvider : public clan::SoundCache
{
public:
	clan::Resource<clan::SoundBuffer> get_sound(const std::string &id) override;

private:
	std::map<std::string, clan::Resource<clan::SoundBuffer>> sounds;
};

inline clan::Resource<clan::SoundBuffer> SoundCacheProvider::get_sound(const std::string &id)
{
	auto it = sounds.find(id);
	if (it != sounds.end())
		return it->second;

	sounds[id] = clan::Resource<clan::SoundBuffer>(clan::SoundBuffer(id));
	return sounds[id];
}