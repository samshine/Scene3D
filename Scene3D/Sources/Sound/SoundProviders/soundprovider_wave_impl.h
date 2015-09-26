
#pragma once

#include "Sound/soundformat.h"

class SoundProvider_Wave_Impl
{
public:
	SoundProvider_Wave_Impl()
		: data(nullptr)
	{
	}

	~SoundProvider_Wave_Impl()
	{
		delete[] data;
	}

	void load(uicore::IODevice &source);

	char *data;
	SoundFormat format;
	int num_channels;
	int num_samples;
	int frequency;

private:
	uint32_t find_subchunk(const char *chunk, uicore::IODevice &source, uint32_t file_offset, uint32_t max_offset);
};
