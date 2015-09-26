
#pragma once

#include <list>

class SoundCache;
class AudioObject_Impl;

class AudioWorld_Impl
{
public:
	AudioWorld_Impl(const std::shared_ptr<SoundCache> &sound_cache);
	~AudioWorld_Impl();

	void update_session(AudioObject_Impl *obj);

	std::list<AudioObject_Impl *> objects;
	std::list<AudioObject> active_objects;

	uicore::Vec3f listener_position;
	uicore::Quaternionf listener_orientation;
	bool play_ambience = true;
	bool reverse_stereo = false;

	std::shared_ptr<SoundCache> sound_cache;
};
