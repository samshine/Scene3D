
#pragma once

class SoundProvider_Raw_Impl
{
public:
	SoundProvider_Raw_Impl() : sound_data(nullptr)
	{
	}

	~SoundProvider_Raw_Impl()
	{
		if (sound_data)
			delete[] sound_data;
	}

	unsigned char *sound_data;
	int num_samples;
	int bytes_per_sample;
	bool stereo;
	int frequency;
};
