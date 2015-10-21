
#pragma once

#include <list>
#include "Sound/AudioWorld/audio_object.h"
#include "Sound/soundbuffer.h"
#include "Sound/soundbuffer_session.h"

class AudioWorldImpl;

class AudioObjectImpl : public AudioObject, public std::enable_shared_from_this<AudioObjectImpl>
{
public:
	AudioObjectImpl(AudioWorldImpl *world);
	~AudioObjectImpl();

	uicore::Vec3f position() const override;

	float attenuation_begin() const override;
	float attenuation_end() const override;
	float volume() const override;
	bool looping() const override;
	bool ambience() const override;
	bool playing() const override;

	void set_position(const uicore::Vec3f &position) override;

	void set_attenuation_begin(float distance) override;
	void set_attenuation_end(float distance) override;
	void set_volume(float volume) override;

	void set_sound(const std::string &id) override;
	void set_sound(const SoundBuffer &buffer) override;

	void set_looping(bool loop) override;
	void set_ambience(bool ambience) override;

	void play() override;
	void stop() override;

	AudioWorldImpl *_world = nullptr;
	std::list<AudioObjectImpl *>::iterator _it;

	uicore::Vec3f _position;
	float _attenuation_begin = 0.0f;
	float _attenuation_end = 0.0f;
	float _volume = 1.0f;
	bool _looping = false;
	bool _ambience = false;
	SoundBuffer _sound;
	SoundBuffer_Session _session;
};
