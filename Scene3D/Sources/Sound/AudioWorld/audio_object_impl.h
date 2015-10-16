
#pragma once

#include <list>
#include "Sound/soundbuffer.h"
#include "Sound/soundbuffer_session.h"

class AudioWorldImpl;

class AudioObjectImpl
{
public:
	AudioObjectImpl(AudioWorldImpl *world);
	~AudioObjectImpl();

	AudioWorldImpl *world;
	std::list<AudioObjectImpl *>::iterator it;

	uicore::Vec3f position;
	float attenuation_begin;
	float attenuation_end;
	float volume;
	bool looping;
	bool ambience;
	SoundBuffer sound;
	SoundBuffer_Session session;
};
