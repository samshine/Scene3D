
#pragma once

#include <vector>
#include "Sound/soundformat.h"
#include "Sound/soundoutput.h"
#include "Sound/soundbuffer.h"
#include <memory>
#include <mutex>

class SoundBufferImpl;
class SoundProvider_Session;
class SoundOutputImpl;

class SoundBuffer_SessionImpl
{
public:
	SoundBuffer_SessionImpl(SoundBuffer &soundbuffer, bool looping, SoundOutput &output);
	virtual ~SoundBuffer_SessionImpl();

	SoundBuffer soundbuffer;
	std::shared_ptr<SoundProvider_Session> provider_session;
	SoundOutput output;
	float volume;
	float frequency;
	float pan;
	bool looping;
	bool playing;
	mutable std::recursive_mutex mutex;

	bool mix_to(float **sample_data, float **temp_data, int num_samples, int num_channels);

private:
	/// \brief Mixes the sample data from 'temp_data' into 'sample_data'
	void mix_channels(int num_channels, int num_samples, float ** sample_data, float ** temp_data);

	/// \brief Returns the volume of left and right channel
	void get_channel_volume(float *out_volume);

	/// \brief Reads data into temp_data in the mixers native frequency
	void get_data_in_mixer_frequency(int num_samples, float **temp_data);

	/// \brief Fills temporary buffers with data from provider.
	void get_data();

	/// \brief Temporary channel buffers containing sound data in provider frequency.
	float **float_buffer_data;

	std::vector<float*> float_buffer_data_offsetted;

	/// \brief Size of temporary channel buffers.
	int num_buffer_samples;

	/// \brief Number of temporary channel buffers;
	int num_buffer_channels;

	/// \brief Current playback position in temporary buffers.
	double buffer_position;

	/// \brief Number of samples currently written to buffer_data.
	int buffer_samples_written;
};
