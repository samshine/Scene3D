
#include "precomp.h"
#include "soundbuffer_session_impl.h"
#include "soundbuffer_impl.h"
#include "soundoutput_impl.h"
#include "Sound/sound_sse.h"
#include "Sound/soundprovider.h"
#include "Sound/soundprovider_session.h"

SoundBuffer_SessionImpl::SoundBuffer_SessionImpl(SoundBuffer &soundbuffer, bool looping, SoundOutput &output)
	: soundbuffer(soundbuffer), provider_session(nullptr), output(output), volume(1.0f), pan(0.0f), looping(looping), playing(false)
{
	volume = soundbuffer.get_volume();
	pan = soundbuffer.get_pan();
	provider_session = soundbuffer.get_provider()->begin_session();
	provider_session->set_looping(looping);
	frequency = (float)provider_session->get_frequency();

	num_buffer_samples = 16 * 1024;
	num_buffer_channels = provider_session->get_num_channels();
	buffer_position = 0.0;
	buffer_samples_written = 0;

	float_buffer_data = new float*[num_buffer_channels];
	for (int i = 0; i < num_buffer_channels; i++) float_buffer_data[i] = new float[num_buffer_samples];

	float_buffer_data_offsetted.resize(num_buffer_channels);
}

SoundBuffer_SessionImpl::~SoundBuffer_SessionImpl()
{
	for (int j = 0; j < num_buffer_channels; ++j) delete[] float_buffer_data[j];
	delete[] float_buffer_data;
}

bool SoundBuffer_SessionImpl::mix_to(float **sample_data, float **temp_data, int num_samples, int num_channels)
{
	std::unique_lock<std::recursive_mutex> mutex_lock(mutex);
	get_data_in_mixer_frequency(num_samples, temp_data);
	mix_channels(num_channels, num_samples, sample_data, temp_data);
	return playing;
}

void SoundBuffer_SessionImpl::get_data()
{
	int num_session_channels = provider_session->get_num_channels();
	if (num_session_channels != num_buffer_channels)
	{
		//log_event("mixer", "Number of session channels does not match the number of buffers");
		return;
	}

	if (num_session_channels > 0)
	{
		// Copy stream data to working buffer:
		int samples_left = num_buffer_samples;
		while (samples_left > 0)
		{
			for (int i = 0; i < num_session_channels; i++)
				float_buffer_data_offsetted[i] = float_buffer_data[i] + num_buffer_samples - samples_left;

			int written = provider_session->get_data(&float_buffer_data_offsetted[0], samples_left);
			samples_left -= written;

			if (samples_left > 0 && provider_session->eof())
			{
				// Reached end of stream, loop if enabled.
				if (looping)
				{
					// Try to loop back to beginning:
					if (provider_session->set_position(0) == false)
					{
						// It failed. Stop playing then.
						break;
					}

					// Start playing again if it stopped by reaching eof.
					provider_session->play();
				}
				else
				{
					break;
				}
			}
			else if (written == 0)
			{
				// No more data to get from provider for some reason.
				break;
			}
		}

		buffer_samples_written = num_buffer_samples - samples_left;
	}
}

void SoundBuffer_SessionImpl::get_data_in_mixer_frequency(int num_samples, float **temp_data)
{
	// Convert from session frequency to mixer frequency:
	// This is done by copying data from the temporary session buffers (buffer_data) to
	// the temporary mixing buffers (temp_data), and if buffer_data is exhausted, calling
	// get_data() to fill it with new data from the soundprovider session object.
	double speed = frequency / double(output.get_mixing_frequency());
	int sample_count;
	for (sample_count = 0; sample_count < num_samples; sample_count++)
	{
		if (buffer_position < buffer_samples_written)
		{
			for (int chan = 0; chan < num_buffer_channels; chan++)
			{
				temp_data[chan][sample_count] = float_buffer_data[chan][int(buffer_position)];
			}
		}
		else
		{
			if (provider_session->eof())
			{
				playing = false;
				break;
			}

			// Out of data, get more from provider:
			buffer_position -= buffer_samples_written;
			get_data();
			sample_count--;
			continue;
		}

		buffer_position += speed;
	}

	// Clear the remaining samples (if any)
	for (; sample_count < num_samples; sample_count++)
	{
		for (int chan = 0; chan < num_buffer_channels; chan++)
		{
			temp_data[chan][sample_count] = 0.0f;
		}
	}
}

void SoundBuffer_SessionImpl::get_channel_volume(float *channel_volume)
{
	float left_pan = 1 - pan;
	float right_pan = 1 + pan;
	if (left_pan < 0.0f) left_pan = 0.0f;
	if (left_pan > 1.0f) left_pan = 1.0f;
	if (right_pan < 0.0f) right_pan = 0.0f;
	if (right_pan > 1.0f) right_pan = 1.0f;
	if (volume < 0.0f) volume = 0.0f;
	if (volume > 1.0f) volume = 1.0f;

	float left_volume = volume * left_pan;
	float right_volume = volume * right_pan;

	channel_volume[0] = left_volume;
	channel_volume[1] = right_volume;
}

void SoundBuffer_SessionImpl::mix_channels(int num_channels, int num_samples, float ** sample_data, float ** temp_data)
{
	float channel_volume[2];
	get_channel_volume(channel_volume);

	if (num_buffer_channels == 1)
	{
		// If its a mono stream, play it in left and right channels:
		SoundSSE::mix_one_to_many(temp_data[0], num_samples, sample_data, channel_volume, 2);
	}
	else
	{
		if (num_channels > num_buffer_channels)
			num_channels = num_buffer_channels;

		for (int chan = 0; chan < num_channels; chan++)
			SoundSSE::mix_one_to_one(temp_data[chan], num_samples, sample_data[chan], channel_volume[chan]);
	}
}
