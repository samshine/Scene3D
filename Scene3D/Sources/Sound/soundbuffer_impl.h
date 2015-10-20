
#pragma once

#include <vector>
#include <mutex>

class SoundProvider;
typedef std::shared_ptr<SoundProvider> SoundProviderPtr;

class SoundBufferImpl
{
public:
	SoundProviderPtr provider;
	float volume = 1.0f;
	float pan = 0.0f;
	mutable std::recursive_mutex mutex;
};
