
#pragma once

#include "Sound/soundprovider_session.h"
#include "soundprovider_raw.h"

class SoundProvider_Raw_Session : public SoundProvider_Session
{
public:
	SoundProvider_Raw_Session(std::shared_ptr<SoundProvider_Raw> source);
	~SoundProvider_Raw_Session();

	int get_num_samples() const override;
	int get_frequency() const override;
	int get_num_channels() const override;
	int get_position() const override;

	bool eof() const override;
	void stop() override;
	bool play() override;
	bool set_position(int pos) override;
	bool set_end_position(int pos) override { return false; }
	int get_data(float **data_ptr, int data_requested) override;

private:
	std::shared_ptr<SoundProvider_Raw> source;

	int position = 0;
	int num_samples = 0;
	int frequency = 0;
};
