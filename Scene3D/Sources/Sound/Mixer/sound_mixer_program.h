
#pragma once

#include "speaker_position.h"
#include "channel_buffers_data.h"

class SoundMixingInput
{
public:
	SoundMixingBuffersData data;
	unsigned int buffer_size;
	float read_cursor;
	unsigned int write_cursor;
	float volumes[32];
	float speed;

	virtual void get_more_data() = 0;
};

class SoundMixerProgram
{
public:
	virtual void mix(SoundMixingBuffersData &output, std::vector<SoundMixingInput*> inputs, unsigned int sample_count) = 0;
};

class StandardSoundMixerProgram : public SoundMixerProgram
{
public:
	void mix(SoundMixingBuffersData &output, std::vector<SoundMixingInput*> inputs, unsigned int sample_count)
	{
		for (unsigned int i = 0; i < inputs.size(); i++)
		{
			SoundMixingInput *input = inputs[i];
			for (unsigned int s = 0; s < sample_count; s++)
			{
				for (int c = 0; c < 32; c++)
				{
					if (output.channels[c])
						if (input->data.channels[c])
							output.channels[c][s] += input->data.channels[c][(unsigned int)input->read_cursor] * input->volumes[c];
				}

				input->read_cursor += input->speed;
				if (input->read_cursor >= input->write_cursor)
					input->get_more_data();
			}
		}
	}
};
