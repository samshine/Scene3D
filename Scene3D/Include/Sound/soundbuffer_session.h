
#pragma once

#include <memory>

class SoundCard;
class SoundBuffer;
class SoundBuffer_Session_Impl;
class SoundOutput;

class SoundBuffer_Session
{
public:
	SoundBuffer_Session();
	virtual ~SoundBuffer_Session();

	bool is_null() const { return !impl; }
	void throw_if_null() const;

	int get_position() const;
	float get_position_relative() const;
	int get_length() const;
	int get_frequency() const;
	float get_volume() const;
	float get_pan() const;
	bool get_looping() const;
	bool is_playing();

	bool set_position(int new_pos);
	bool set_position_relative(float new_pos);
	bool set_end_position(int pos);
	void set_frequency(int new_freq);
	void set_volume(float new_volume);
	void set_pan(float new_pan);
	void set_looping(bool loop);

	void play();
	void stop();

private:
	SoundBuffer_Session(SoundBuffer &soundbuffer, bool looping, SoundOutput &output);
	std::shared_ptr<SoundBuffer_Session_Impl> impl;

	friend class SoundBuffer;
	friend class SoundOutput_Impl;
};
