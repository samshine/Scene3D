
#pragma once

#include <memory>
#include "soundbuffer_session.h"

class SoundOutput;
class SoundProvider;
class SoundBuffer_Session;
class SoundBuffer_Impl;

class SoundBuffer
{
public:
	SoundBuffer();
	SoundBuffer(SoundProvider *provider);
	SoundBuffer(const std::string &fullname, bool streamed = false, const std::string &format = "");
	SoundBuffer(const std::string &filename, bool streamed, const uicore::FileSystem &fs, const std::string &type = "");
	SoundBuffer(uicore::IODevice &file, bool streamed, const std::string &type);
	~SoundBuffer();

	SoundProvider *get_provider() const;

	float get_volume() const;
	float get_pan() const;

	bool is_null() const { return !impl; }
	void throw_if_null() const;

	void set_volume(float new_volume);
	void set_pan(float new_pan);

	SoundBuffer_Session play(bool looping = false, SoundOutput *output = nullptr);
	SoundBuffer_Session prepare(bool looping = false, SoundOutput *output = nullptr);

private:
	std::shared_ptr<SoundBuffer_Impl> impl;
};
