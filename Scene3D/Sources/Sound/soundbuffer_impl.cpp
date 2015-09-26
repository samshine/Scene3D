
#include "precomp.h"
#include "soundbuffer_impl.h"
#include "Sound/SoundProviders/soundprovider.h"

SoundBuffer_Impl::SoundBuffer_Impl() : provider(nullptr), volume(1.0f), pan(0.0f)
{
}

SoundBuffer_Impl::~SoundBuffer_Impl()
{
	if (provider)
		delete provider;
}
