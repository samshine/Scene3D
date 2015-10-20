
#include "precomp.h"
#include "soundprovider_wave_session.h"
#include "soundprovider_wave.h"
#include "Sound/soundformat.h"
#include "Sound/sound_sse.h"

using namespace uicore;

SoundProvider_Wave_Session::SoundProvider_Wave_Session(std::shared_ptr<SoundProvider_Wave>source) : source(source), position(0)
{
	frequency = source->frequency;
	end_position = num_samples = source->num_samples;
}

SoundProvider_Wave_Session::~SoundProvider_Wave_Session()
{
}

int SoundProvider_Wave_Session::get_num_samples() const
{
	return num_samples;
}

int SoundProvider_Wave_Session::get_frequency() const
{
	return frequency;
}

int SoundProvider_Wave_Session::get_num_channels() const
{
	return source->num_channels;
}

int SoundProvider_Wave_Session::get_position() const
{
	return position;
}

bool SoundProvider_Wave_Session::eof() const
{
	return (position >= end_position);
}

void SoundProvider_Wave_Session::stop()
{
}

bool SoundProvider_Wave_Session::play()
{
	return true;
}

bool SoundProvider_Wave_Session::set_position(int pos)
{
	position = pos;
	return true;
}

bool SoundProvider_Wave_Session::set_end_position(int pos)
{
	if (pos > num_samples)
		throw Exception("Attempted to set the sample end position higher than the number of samples");
	end_position = pos;
	return true;
}

int SoundProvider_Wave_Session::get_data(float **data_ptr, int data_requested)
{
	int block_start = position;
	int block_end = position + data_requested;
	if (block_end > end_position)
		block_end = end_position;

	int retrieved = block_end - block_start;

	if (source->format == sf_16bit_signed)
	{
		if (source->num_channels == 2)
		{
			short *src = source->data->data<short>() + position * 2;
			SoundSSE::unpack_16bit_stereo(src, retrieved * 2, data_ptr);
		}
		else
		{
			short *src = source->data->data<short>() + position;
			SoundSSE::unpack_16bit_mono(src, retrieved, data_ptr[0]);
		}
	}
	else
	{
		if (source->num_channels == 2)
		{
			unsigned char *src = source->data->data<unsigned char>() + position * 2;
			SoundSSE::unpack_8bit_stereo(src, retrieved * 2, data_ptr);
		}
		else
		{
			unsigned char *src = source->data->data<unsigned char>() + position;
			SoundSSE::unpack_8bit_mono(src, retrieved, data_ptr[0]);
		}
	}

	position += retrieved;
	return retrieved;
}
