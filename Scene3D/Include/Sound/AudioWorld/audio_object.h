
#pragma once

#include <string>

class SoundBuffer;
class AudioWorld;
class AudioObjectImpl;

class AudioObject
{
public:
	AudioObject();
	AudioObject(AudioWorld &world);

	bool is_null() const { return !impl; }

	uicore::Vec3f get_position() const;

	float get_attenuation_begin() const;
	float get_attenuation_end() const;
	float get_volume() const;
	bool is_looping() const;
	bool is_ambience() const;
	bool is_playing() const;

	void set_position(const uicore::Vec3f &position);

	void set_attenuation_begin(float distance);
	void set_attenuation_end(float distance);
	void set_volume(float volume);

	void set_sound(const std::string &id);
	void set_sound(const SoundBuffer &buffer);

	void set_looping(bool loop);
	void set_ambience(bool ambience);

	void play();
	void stop();

private:
	std::shared_ptr<AudioObjectImpl> impl;

	friend class AudioWorld;
	friend class AudioWorldImpl;
};
