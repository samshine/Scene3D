
#pragma once

#include "sound_sample_source.h"

class SoundMixer : public SoundSampleSource
{
public:
	SoundMixer(int frequency, SoundMixerProgram *program);

	void add_input_source(SoundSampleSource *input_source);
	void remove_input_source(SoundSampleSource *input_source);

	int get_frequency() const { return frequency; }
	void get_data(SoundMixingBuffersData &output, int sample_count);

private:
	int frequency;
	SoundMixerProgram *program;
	std::vector<SoundMixingBuffersContainer *> inputs;
	std::vector<SoundSampleSource *> sources;
};
