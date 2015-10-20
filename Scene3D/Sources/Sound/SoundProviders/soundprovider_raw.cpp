
#include "precomp.h"
#include "soundprovider_raw.h"
#include "soundprovider_raw_session.h"

SoundProvider_Raw::SoundProvider_Raw(const uicore::DataBufferPtr &sound_data, int bytes_per_sample, bool stereo, int frequency) : sound_data(sound_data)
{
}

std::shared_ptr<SoundProvider_Session> SoundProvider_Raw::begin_session()
{
	return std::make_shared<SoundProvider_Raw_Session>(shared_from_this());
}
