
#pragma once

#include "Sound/SoundProviders/soundprovider_session.h"
#include "Sound/SoundProviders/soundprovider_wave.h"

class SoundProvider_Wave_Session : public SoundProvider_Session
{
public:
	SoundProvider_Wave_Session(SoundProvider_Wave &source);
	~SoundProvider_Wave_Session();

	int get_num_samples() const override;
	int get_frequency() const override;
	int get_num_channels() const override;
	int get_position() const override;

	bool eof() const override;
	void stop() override;
	bool play() override;
	bool set_position(int pos) override;
	bool set_end_position(int pos) override;
	int get_data(float **data_ptr, int data_requested) override;

private:
	SoundProvider_Wave source;

	int position;
	int end_position;
	int num_samples;
	int frequency;
};
