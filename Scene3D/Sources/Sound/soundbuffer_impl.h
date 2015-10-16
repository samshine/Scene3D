
#pragma once

#include <vector>
#include <mutex>

class SoundProvider;

class SoundBufferImpl
{
public:
	SoundBufferImpl();
	virtual ~SoundBufferImpl();

	SoundProvider *provider;
	float volume;
	float pan;
	mutable std::recursive_mutex mutex;
};
