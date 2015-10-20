
#include "precomp.h"
#include "Sound/soundprovider.h"
#include "SoundProviders/soundprovider_raw.h"
#include "SoundProviders/soundprovider_vorbis.h"
#include "SoundProviders/soundprovider_wave.h"

using namespace uicore;

std::shared_ptr<SoundProvider> SoundProvider::file(const std::string &filename, bool streamed)
{
	return file(File::open_existing(filename), streamed, PathHelp::get_extension(filename));
}

std::shared_ptr<SoundProvider> SoundProvider::file(const uicore::IODevicePtr &file, bool streamed, const std::string &type)
{
	if (Text::equal_caseless(type, "wav"))
		return wave(file, streamed);
	else if (Text::equal_caseless(type, "ogg"))
		return vorbis(file, streamed);
	else
		throw Exception("Unknown sound file type");
}

std::shared_ptr<SoundProvider> SoundProvider::raw(const DataBufferPtr &sound_data, int bytes_per_sample, bool stereo, int frequency)
{
	return std::make_shared<SoundProvider_Raw>(sound_data, bytes_per_sample, stereo, frequency);
}

std::shared_ptr<SoundProvider> SoundProvider::vorbis(const std::string &filename, bool stream)
{
	return vorbis(File::open_existing(filename), stream);
}

std::shared_ptr<SoundProvider> SoundProvider::vorbis(const uicore::IODevicePtr &file, bool stream)
{
	return std::make_shared<SoundProvider_Vorbis>(file, stream);
}

std::shared_ptr<SoundProvider> SoundProvider::wave(const std::string &filename, bool stream)
{
	return wave(File::open_existing(filename), stream);
}

std::shared_ptr<SoundProvider> SoundProvider::wave(const uicore::IODevicePtr &file, bool stream)
{
	return std::make_shared<SoundProvider_Wave>(file, stream);
}
