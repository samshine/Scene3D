
#pragma once

class SoundMixingBuffersData;

class SoundSampleSource
{
public:
	virtual int get_frequency() const = 0;
	virtual void get_data(SoundMixingBuffersData &output, int sample_count) = 0;
};
