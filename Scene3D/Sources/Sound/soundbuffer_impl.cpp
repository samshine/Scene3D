
#include "precomp.h"
#include "soundbuffer_impl.h"
#include "Sound/SoundProviders/soundprovider.h"

SoundBufferImpl::SoundBufferImpl() : provider(nullptr), volume(1.0f), pan(0.0f)
{
}

SoundBufferImpl::~SoundBufferImpl()
{
	if (provider)
		delete provider;
}
