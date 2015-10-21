
#pragma once

#include "Sound/AudioWorld/audio_world.h"
#include <list>

class SoundCache;
class AudioObject;
typedef std::shared_ptr<AudioObject> AudioObjectPtr;
class AudioObjectImpl;

class AudioWorldImpl : public AudioWorld
{
public:
	AudioWorldImpl(const std::shared_ptr<SoundCache> &sound_cache);

	void update() override;

	uicore::Vec3f listener_position() override { return _listener_position; }
	uicore::Quaternionf listener_orientation() override { return _listener_orientation; }
	void set_listener(const uicore::Vec3f &position, const uicore::Quaternionf &orientation) override;

	void set_ambience_enabled(bool enable) override;
	bool ambience_enabled() const override;

	void set_reverse_stereo(bool enable) override;
	bool reverse_stereo() const override;

	void update_session(AudioObjectImpl *obj);

	std::list<AudioObjectImpl *> objects;
	std::list<AudioObjectPtr> active_objects;

	uicore::Vec3f _listener_position;
	uicore::Quaternionf _listener_orientation;
	bool _play_ambience = true;
	bool _reverse_stereo = false;

	std::shared_ptr<SoundCache> sound_cache;
};
