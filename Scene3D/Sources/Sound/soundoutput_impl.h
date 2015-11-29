
#pragma once

#include <vector>
#include <list>
#include <memory>
#include <mutex>
#include <thread>
#include <atomic>

class SoundBuffer_SessionImpl;
class SoundBuffer_Session;

class SoundOutputImpl
{
public:
	SoundOutputImpl(int mixing_frequency, int mixing_latency);
	virtual ~SoundOutputImpl();

	void play_session(SoundBuffer_Session &session);
	void stop_session(SoundBuffer_Session &session);

protected:
	std::string name;
	int mixing_frequency;
	int mixing_latency;
	float volume;
	float pan;
	std::thread thread;
	std::atomic_bool stop_flag;
	std::vector< SoundBuffer_Session > sessions;

	int mix_buffer_size;
	std::vector<float *> mix_buffers;
	std::vector<float *> temp_buffers;
	float *packed_buffer;
	int num_channels = 2;

	/// \brief Called when we have no samples to play - and wants to tell the soundcard
	/// \brief about this possible event.
	virtual void silence() = 0;

	/// \brief Returns the buffer size used by device (returned as num [stereo] samples).
	virtual int get_fragment_size() = 0;

	/// \brief Writes a fragment to the soundcard.
	virtual void write_fragment(float *data) = 0;

	/// \brief Waits until output source isn't full anymore.
	virtual void wait() = 0;

	/// \brief Called by the mixer thread when it starts
	virtual void mixer_thread_starting() { }

	/// \brief Called by the mixer thread when it stops
	virtual void mixer_thread_stopping() { }

	/// \brief Starts a thread and call mix_fragment() and wait() continueously.
	void start_mixer_thread();

	/// \brief Stops the mixer thread.
	void stop_mixer_thread();

	/// \brief Mixes a single fragment and stores the result in packed_buffer.
	void mix_fragment();

private:
	/// \brief Worker thread for output device. Mixes the audio and sends it to write_fragment.
	void mixer_thread();

	/// \brief Returns true if the mixer thread should continue mixing fragments
	bool if_continue_mixing();

	/// \brief Ensures the mixing buffers match the fragment size
	void resize_mix_buffers();

	/// \brief Clears the content of the mixing buffers
	void clear_mix_buffers();

	/// \brief Mixes soundbuffer sessions into the mixing buffers
	void fill_mix_buffers();

	/// \brief Apply master volume and panning to mix buffers
	void apply_master_volume_on_mix_buffers();

	/// \brief Clamp mixing buffer values to the -1 to 1 range
	void clamp_mix_buffers();

	static std::recursive_mutex singleton_mutex;
	static SoundOutputImpl *instance;

	mutable std::recursive_mutex mutex;

	friend class SoundOutput;
};
