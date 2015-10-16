
#pragma once

#include <list>

class SoundCache;
class AudioObjectImpl;

class AudioWorldImpl
{
public:
	AudioWorldImpl(const std::shared_ptr<SoundCache> &sound_cache);
	~AudioWorldImpl();

	void update_session(AudioObjectImpl *obj);

	std::list<AudioObjectImpl *> objects;
	std::list<AudioObject> active_objects;

	uicore::Vec3f listener_position;
	uicore::Quaternionf listener_orientation;
	bool play_ambience = true;
	bool reverse_stereo = false;

	std::shared_ptr<SoundCache> sound_cache;
};
