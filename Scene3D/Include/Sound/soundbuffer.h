
#pragma once

#include <memory>
#include "soundbuffer_session.h"

class SoundOutput;
class SoundProvider;
typedef std::shared_ptr<SoundProvider> SoundProviderPtr;
class SoundBuffer_Session;
class SoundBufferImpl;

class SoundBuffer
{
public:
	SoundBuffer();
	SoundBuffer(const SoundProviderPtr &provider);
	SoundBuffer(const std::string &filename, bool streamed = false);
	SoundBuffer(const uicore::IODevicePtr &file, bool streamed, const std::string &format);
	~SoundBuffer();

	SoundProviderPtr get_provider() const;

	float get_volume() const;
	float get_pan() const;

	bool is_null() const { return !impl; }
	void throw_if_null() const;

	void set_volume(float new_volume);
	void set_pan(float new_pan);

	SoundBuffer_Session play(bool looping = false, SoundOutput *output = nullptr);
	SoundBuffer_Session prepare(bool looping = false, SoundOutput *output = nullptr);

private:
	std::shared_ptr<SoundBufferImpl> impl;
};
