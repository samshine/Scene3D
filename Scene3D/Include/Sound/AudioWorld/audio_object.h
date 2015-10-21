
#pragma once

#include <string>

class SoundBuffer;
class AudioWorld;
typedef std::shared_ptr<AudioWorld> AudioWorldPtr;

class AudioObject
{
public:
	static std::shared_ptr<AudioObject> create(const AudioWorldPtr &world);

	virtual uicore::Vec3f position() const = 0;

	virtual float attenuation_begin() const = 0;
	virtual float attenuation_end() const = 0;
	virtual float volume() const = 0;
	virtual bool looping() const = 0;
	virtual bool ambience() const = 0;
	virtual bool playing() const = 0;

	virtual void set_position(const uicore::Vec3f &position) = 0;

	virtual void set_attenuation_begin(float distance) = 0;
	virtual void set_attenuation_end(float distance) = 0;
	virtual void set_volume(float volume) = 0;

	virtual void set_sound(const std::string &id) = 0;
	virtual void set_sound(const SoundBuffer &buffer) = 0;

	virtual void set_looping(bool loop) = 0;
	virtual void set_ambience(bool ambience) = 0;

	virtual void play() = 0;
	virtual void stop() = 0;
};

typedef std::shared_ptr<AudioObject> AudioObjectPtr;
