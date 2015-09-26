
#include "precomp.h"
#include "Sound/SoundProviders/soundprovider_raw.h"
#include "soundprovider_raw_impl.h"
#include "soundprovider_raw_session.h"

SoundProvider_Raw::SoundProvider_Raw(void *sound_data, int num_samples, int bytes_per_sample, bool stereo, int frequency) : impl(std::make_shared<SoundProvider_Raw_Impl>())
{
	int data_size = num_samples * bytes_per_sample;
	if (stereo) data_size *= 2;

	impl->sound_data = new unsigned char[data_size];
	memcpy(impl->sound_data, sound_data, data_size);
	impl->num_samples = num_samples;
	impl->bytes_per_sample = bytes_per_sample;
	impl->stereo = stereo;
	impl->frequency = frequency;
}

SoundProvider_Raw::~SoundProvider_Raw()
{
}

SoundProvider_Session *SoundProvider_Raw::begin_session()
{
	return new SoundProvider_Raw_Session(*this);
}

void SoundProvider_Raw::end_session(SoundProvider_Session *session)
{
	delete session;
}
