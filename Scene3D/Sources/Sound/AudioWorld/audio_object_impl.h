
#pragma once

#include <list>
#include "Sound/soundbuffer.h"
#include "Sound/soundbuffer_session.h"

class AudioWorld_Impl;

class AudioObject_Impl
{
public:
	AudioObject_Impl(AudioWorld_Impl *world);
	~AudioObject_Impl();

	AudioWorld_Impl *world;
	std::list<AudioObject_Impl *>::iterator it;

	uicore::Vec3f position;
	float attenuation_begin;
	float attenuation_end;
	float volume;
	bool looping;
	bool ambience;
	SoundBuffer sound;
	SoundBuffer_Session session;
};
