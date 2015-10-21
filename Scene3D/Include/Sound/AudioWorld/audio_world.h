
#pragma once

class SoundCache;

class AudioWorld
{
public:
	static std::shared_ptr<AudioWorld> create(const std::shared_ptr<SoundCache> &sound_cache);

	virtual void update() = 0;

	virtual uicore::Vec3f listener_position() = 0;
	virtual uicore::Quaternionf listener_orientation() = 0;
	virtual void set_listener(const uicore::Vec3f &position, const uicore::Quaternionf &orientation) = 0;

	virtual void set_ambience_enabled(bool enable) = 0;
	virtual bool ambience_enabled() const = 0;

	virtual void set_reverse_stereo(bool enable) = 0;
	virtual bool reverse_stereo() const = 0;
};

typedef std::shared_ptr<AudioWorld> AudioWorldPtr;
