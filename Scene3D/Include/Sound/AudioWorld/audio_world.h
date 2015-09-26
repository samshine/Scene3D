
#pragma once

class SoundBuffer;
class SoundCache;
class AudioWorld_Impl;

class AudioWorld
{
public:
	AudioWorld(const std::shared_ptr<SoundCache> &sound_cache);

	void update();

	void set_listener(const uicore::Vec3f &position, const uicore::Quaternionf &orientation);

	void enable_ambience(bool enable);
	bool is_ambience_enabled() const;

	void enable_reverse_stereo(bool enable);
	bool is_reverse_stereo_enabled() const;

private:
	std::shared_ptr<AudioWorld_Impl> impl;

	friend class AudioObject;
	friend class AudioObject_Impl;
};
