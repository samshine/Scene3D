
#include "precomp.h"
#include "soundprovider_raw_session.h"
#include "soundprovider_raw.h"
#include "Sound/soundformat.h"
#include "Sound/sound_sse.h"

SoundProvider_Raw_Session::SoundProvider_Raw_Session(std::shared_ptr<SoundProvider_Raw> source) : source(source)
{
	frequency = source->frequency;
	num_samples = source->sound_data->size() / source->bytes_per_sample;
	if (source->stereo)
		num_samples /= 2;
}

SoundProvider_Raw_Session::~SoundProvider_Raw_Session()
{
}

int SoundProvider_Raw_Session::get_num_samples() const
{
	return num_samples;
}

int SoundProvider_Raw_Session::get_frequency() const
{
	return frequency;
}

int SoundProvider_Raw_Session::get_num_channels() const
{
	return (source->stereo) ? 2 : 1;
}

int SoundProvider_Raw_Session::get_position() const
{
	return position;
}

bool SoundProvider_Raw_Session::eof() const
{
	return (position >= num_samples);
}

void SoundProvider_Raw_Session::stop()
{
}

bool SoundProvider_Raw_Session::play()
{
	return true;
}

bool SoundProvider_Raw_Session::set_position(int pos)
{
	position = pos;
	return true;
}

int SoundProvider_Raw_Session::get_data(float **data_ptr, int data_requested)
{
	if (position + data_requested > num_samples)
	{
		data_requested = num_samples - position;
		if (data_requested < 0) return 0;
	}

	if (source->bytes_per_sample == 2)
	{
		if (source->stereo)
		{
			short *src = source->sound_data->data<short>() + position * source->bytes_per_sample;
			SoundSSE::unpack_16bit_stereo(src, data_requested, data_ptr);
		}
		else
		{
			short *src = source->sound_data->data<short>() + position * source->bytes_per_sample;
			SoundSSE::unpack_16bit_mono(src, data_requested, data_ptr[0]);
		}
	}
	else if (source->bytes_per_sample == 1)
	{
		if (source->stereo)
		{
			unsigned char *src = source->sound_data->data<unsigned char>() + position * source->bytes_per_sample;
			SoundSSE::unpack_8bit_stereo(src, data_requested, data_ptr);
		}
		else
		{
			unsigned char *src = source->sound_data->data<unsigned char>() + position * source->bytes_per_sample;
			SoundSSE::unpack_8bit_mono(src, data_requested, data_ptr[0]);
		}
	}

	position += data_requested;
	return data_requested;
}
