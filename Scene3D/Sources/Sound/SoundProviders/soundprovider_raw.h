
#pragma once

#include "Sound/soundprovider.h"

class SoundProvider_Raw : public SoundProvider, public std::enable_shared_from_this<SoundProvider_Raw>
{
public:
	SoundProvider_Raw(const uicore::DataBufferPtr &sound_data, int bytes_per_sample, bool stereo, int frequency);

	std::shared_ptr<SoundProvider_Session> begin_session() override;

	uicore::DataBufferPtr sound_data;
	int bytes_per_sample = 0;
	bool stereo = false;
	int frequency = 0;
};
