
#pragma once

#include "Sound/SoundProviders/soundprovider_session.h"
#include "Sound/SoundProviders/soundprovider_vorbis.h"
#include "stb_vorbis.h"

class SoundProvider_Vorbis_Session : public SoundProvider_Session
{
public:
	SoundProvider_Vorbis_Session(SoundProvider_Vorbis &source);
	~SoundProvider_Vorbis_Session();

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
	SoundProvider_Vorbis source;
	int position;
	bool stream_eof;

	stb_vorbis *handle;
	stb_vorbis_info stream_info;
	int stream_byte_offset;

	float **pcm;
	int pcm_position;
	int pcm_samples;
};
