
#pragma once

#include <memory>

class SoundProvider_Session;

class SoundProvider
{
public:
	static std::shared_ptr<SoundProvider> file(const std::string &filename, bool streamed = false);
	static std::shared_ptr<SoundProvider> file(const uicore::IODevicePtr &file, bool streamed, const std::string &type);

	static std::shared_ptr<SoundProvider> raw(const uicore::DataBufferPtr &sound_data, int bytes_per_sample, bool stereo, int frequency);
	static std::shared_ptr<SoundProvider> vorbis(const std::string &filename, bool stream = false);
	static std::shared_ptr<SoundProvider> vorbis(const uicore::IODevicePtr &file, bool stream = false);
	static std::shared_ptr<SoundProvider> wave(const std::string &filename, bool stream = false);
	static std::shared_ptr<SoundProvider> wave(const uicore::IODevicePtr &file, bool stream = false);

	virtual std::shared_ptr<SoundProvider_Session> begin_session() = 0;
};

typedef std::shared_ptr<SoundProvider> SoundProviderPtr;
