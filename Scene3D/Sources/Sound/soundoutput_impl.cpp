
#include "precomp.h"
#include "soundoutput_impl.h"
#include "soundbuffer_session_impl.h"
#include <algorithm>
#include "Sound/sound_sse.h"

using namespace uicore;

std::recursive_mutex SoundOutputImpl::singleton_mutex;
SoundOutputImpl *SoundOutputImpl::instance = nullptr;

SoundOutputImpl::SoundOutputImpl(int mixing_frequency, int latency)
	: mixing_frequency(mixing_frequency), mixing_latency(latency), volume(1.0f),
	pan(0.0f), mix_buffer_size(0)
{
	packed_buffer = nullptr;

	std::unique_lock<std::recursive_mutex> lock(singleton_mutex);
	if (instance)
		throw Exception("Only a single instance of SoundOutput is allowed");
	instance = this;

}

SoundOutputImpl::~SoundOutputImpl()
{
	SoundSSE::aligned_free(packed_buffer);
	for (auto &mix_buffer : mix_buffers)
		SoundSSE::aligned_free(mix_buffer);
	mix_buffers.clear();
	for (auto &temp_buffer : temp_buffers)
		SoundSSE::aligned_free(temp_buffer);
	temp_buffers.clear();

	std::unique_lock<std::recursive_mutex> lock(singleton_mutex);
	instance = nullptr;
}

void SoundOutputImpl::play_session(SoundBuffer_Session &session)
{
	std::unique_lock<std::recursive_mutex> mutex_lock(mutex);
	sessions.push_back(session);
}

void SoundOutputImpl::stop_session(SoundBuffer_Session &session)
{
	std::unique_lock<std::recursive_mutex> mutex_lock(mutex);

	for (auto it = sessions.begin(); it != sessions.end(); ++it)
	{
		if (session.impl.get() == it->impl.get())
		{
			sessions.erase(it);
			break;
		}
	}
}

void SoundOutputImpl::start_mixer_thread()
{
	stop_flag = false;
	thread = std::thread(&SoundOutputImpl::mixer_thread, this);
	//	thread.set_priority(cl_priority_highest);
}

void SoundOutputImpl::stop_mixer_thread()
{
	std::unique_lock<std::recursive_mutex> mutex_lock(mutex);
	stop_flag = true;
	mutex_lock.unlock();
	thread.join();
	thread = std::thread();
}

void SoundOutputImpl::mix_fragment()
{
	resize_mix_buffers();
	clear_mix_buffers();
	fill_mix_buffers();
	apply_master_volume_on_mix_buffers();
	clamp_mix_buffers();
	SoundSSE::pack_float(mix_buffers.data(), mix_buffer_size, packed_buffer, mix_buffers.size());
}

void SoundOutputImpl::mixer_thread()
{
	mixer_thread_starting();

	while (if_continue_mixing())
	{
		// Mix some audio:
		mix_fragment();

		// Send mixed data to sound card:
		write_fragment(packed_buffer);

		// Wait for sound card to want more:
		wait();
	}

	mixer_thread_stopping();
}

bool SoundOutputImpl::if_continue_mixing()
{
	return !stop_flag;
}

void SoundOutputImpl::resize_mix_buffers()
{
	if (get_fragment_size() != mix_buffer_size)
	{
		SoundSSE::aligned_free(packed_buffer);
		packed_buffer = nullptr;

		for (auto &mix_buffer : mix_buffers)
			SoundSSE::aligned_free(mix_buffer);
		mix_buffers.clear();
		for (auto &temp_buffer : temp_buffers)
			SoundSSE::aligned_free(temp_buffer);
		temp_buffers.clear();

		mix_buffer_size = get_fragment_size();
		//if (mix_buffer_size & 3)
		//	throw Exception("Fragment size must be a multiple of 4");

		for (int i = 0; i < num_channels; i++)
		{
			auto mix_buffer = (float *)SoundSSE::aligned_alloc(sizeof(float) * mix_buffer_size);
			auto temp_buffer = (float *)SoundSSE::aligned_alloc(sizeof(float) * mix_buffer_size);
			SoundSSE::set_float(mix_buffer, mix_buffer_size, 0.0f);
			SoundSSE::set_float(temp_buffer, mix_buffer_size, 0.0f);
			mix_buffers.push_back(mix_buffer);
			temp_buffers.push_back(temp_buffer);
		}
		packed_buffer = (float *)SoundSSE::aligned_alloc(sizeof(float) * mix_buffer_size * num_channels);
		SoundSSE::set_float(packed_buffer, mix_buffer_size * num_channels, 0.0f);
	}
}

void SoundOutputImpl::clear_mix_buffers()
{
	// Clear channel mixing buffers:
	for (auto &mix_buffer : mix_buffers)
		SoundSSE::set_float(mix_buffer, mix_buffer_size, 0.0f);
}

void SoundOutputImpl::fill_mix_buffers()
{
	std::unique_lock<std::recursive_mutex> mutex_lock(mutex);
	std::vector< SoundBuffer_Session > ended_sessions;
	std::vector< SoundBuffer_Session >::iterator it;
	for (it = sessions.begin(); it != sessions.end(); ++it)
	{
		SoundBuffer_Session session = *it;
		bool playing = session.impl->mix_to(mix_buffers.data(), temp_buffers.data(), mix_buffer_size, 2);
		if (!playing) ended_sessions.push_back(session);
	}

	// Release any sessions pending for removal:
	int size_ended_sessions = ended_sessions.size();
	for (int i = 0; i < size_ended_sessions; i++) stop_session(ended_sessions[i]);
}

void SoundOutputImpl::apply_master_volume_on_mix_buffers()
{
	// Calculate volume on left and right channel:
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

	SoundSSE::multiply_float(mix_buffers[0], mix_buffer_size, left_volume);
	SoundSSE::multiply_float(mix_buffers[1], mix_buffer_size, right_volume);
}

void SoundOutputImpl::clamp_mix_buffers()
{
	// Make sure values stay inside 16 bit range:
	for (auto & elem : mix_buffers)
	{
		for (int k = 0; k < mix_buffer_size; k++)
		{
			if (elem[k] > 1.0f)  elem[k] = 1.0f;
			else if (elem[k] < -1.0f) elem[k] = -1.0f;
		}
	}
}
