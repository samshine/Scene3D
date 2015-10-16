
#pragma once

#include <memory>

class SoundBuffer;
class SoundOutput_Description;
class SoundOutputImpl;

class SoundOutput
{
public:
	SoundOutput();
	SoundOutput(int mixing_frequency, int latency = 50);
	SoundOutput(const SoundOutput_Description &desc);
	virtual ~SoundOutput();

	bool is_null() const { return !impl; }
	void throw_if_null() const;

	const std::string &get_name() const;
	int get_mixing_frequency() const;
	int get_mixing_latency() const;
	float get_global_volume() const;
	float get_global_pan() const;

	void stop_all();

	void set_global_volume(float volume);
	void set_global_pan(float pan);

private:
	SoundOutput(const std::weak_ptr<SoundOutputImpl> impl);

	std::shared_ptr<SoundOutputImpl> impl;

	friend class SoundBuffer;
	friend class Sound;
	friend class SoundBuffer_Session;
};
