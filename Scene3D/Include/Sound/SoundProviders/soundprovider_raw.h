
#pragma once

#include "soundprovider.h"

class SoundProvider_Raw_Impl;

class SoundProvider_Raw : public SoundProvider
{
public:
	SoundProvider_Raw(void *sound_data, int num_samples, int bytes_per_sample, bool stereo, int frequency = 22050);
	~SoundProvider_Raw();

	virtual SoundProvider_Session *begin_session() override;
	virtual void end_session(SoundProvider_Session *session) override;

private:
	std::shared_ptr<SoundProvider_Raw_Impl> impl;

	friend class SoundProvider_Raw_Session;
};
