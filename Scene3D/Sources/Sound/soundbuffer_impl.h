
#pragma once

#include <vector>
#include <mutex>

class SoundProvider;

class SoundBuffer_Impl
{
public:
	SoundBuffer_Impl();
	virtual ~SoundBuffer_Impl();

	SoundProvider *provider;
	float volume;
	float pan;
	mutable std::recursive_mutex mutex;
};
