
#include "precomp.h"
#include "soundprovider_raw_session.h"
#include "soundprovider_raw_impl.h"
#include "Sound/soundformat.h"
#include "Sound/sound_sse.h"

SoundProvider_Raw_Session::SoundProvider_Raw_Session(SoundProvider_Raw &source) :
	source(source), position(0)
{
	frequency = source.impl->frequency;
	num_samples = source.impl->num_samples;
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
	return (source.impl->stereo) ? 2 : 1;
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

	if (source.impl->bytes_per_sample == 2)
	{
		if (source.impl->stereo)
		{
			short *src = (short *)source.impl->sound_data + position * source.impl->bytes_per_sample;
			SoundSSE::unpack_16bit_stereo(src, data_requested, data_ptr);
		}
		else
		{
			short *src = (short *)source.impl->sound_data + position * source.impl->bytes_per_sample;
			SoundSSE::unpack_16bit_mono(src, data_requested, data_ptr[0]);
		}
	}
	else if (source.impl->bytes_per_sample == 1)
	{
		if (source.impl->stereo)
		{
			unsigned char *src = (unsigned char *)source.impl->sound_data + position * source.impl->bytes_per_sample;
			SoundSSE::unpack_8bit_stereo(src, data_requested, data_ptr);
		}
		else
		{
			unsigned char *src = (unsigned char *)source.impl->sound_data + position * source.impl->bytes_per_sample;
			SoundSSE::unpack_8bit_mono(src, data_requested, data_ptr[0]);
		}
	}

	position += data_requested;
	return data_requested;
}
