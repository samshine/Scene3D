
#pragma once

#include "channel_buffers_data.h"

class SoundMixingBuffersContainer : public SoundMixingBuffersData
{
public:
	SoundMixingBuffersContainer(SpeakerPositionMask input_speakers, int sample_count)
	{
		for (int i = 0; input_speakers != 0; (input_speakers >>= 1), i++)
		{
			if (input_speakers & 1)
				channels[i] = new float[sample_count];
			else
				channels[i] = 0;
		}
	}

	~SoundMixingBuffersContainer()
	{
		for (int i = 0; i < 32; i++)
			delete[] channels[i];
	}

private:
	SoundMixingBuffersContainer(const SoundMixingBuffersContainer &source) { }
	SoundMixingBuffersContainer &operator =(const SoundMixingBuffersContainer &source) { }
};
