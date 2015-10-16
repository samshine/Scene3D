
#pragma once

#include <memory>
#include "../soundformat.h"

class SoundProvider_SessionImpl;

class SoundProvider_Session
{
public:
	SoundProvider_Session();
	virtual ~SoundProvider_Session();

	virtual int get_num_samples() const = 0;
	virtual int get_frequency() const = 0;
	virtual int get_position() const = 0;
	virtual int get_num_channels() const = 0;
	virtual bool set_looping(bool loop) { return false; }
	virtual bool eof() const = 0;
	virtual void stop() = 0;
	virtual bool play() = 0;
	virtual bool set_position(int pos) = 0;
	virtual bool set_end_position(int pos) = 0;
	virtual int get_data(float **data_ptr, int data_requested) = 0;
};
