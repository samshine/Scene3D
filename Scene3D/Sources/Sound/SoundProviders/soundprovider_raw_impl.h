
#pragma once

class SoundProvider_RawImpl
{
public:
	SoundProvider_RawImpl() : sound_data(nullptr)
	{
	}

	~SoundProvider_RawImpl()
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
