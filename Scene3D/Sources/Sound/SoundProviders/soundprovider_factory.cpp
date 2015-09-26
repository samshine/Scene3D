
#include "precomp.h"

#ifdef _MSC_VER
#pragma warning(disable : 4786)
#endif

#include "Sound/SoundProviders/soundprovider_factory.h"
#include "Sound/SoundProviders/soundprovider_type.h"
#include "../setupsound.h"

using namespace uicore;

SoundProvider *SoundProviderFactory::load(
	const std::string &filename,
	bool streamed,
	const FileSystem &fs,
	const std::string &type)
{
	SetupSound::start();
	auto &types = *SetupSound::get_sound_provider_factory_types();

	if (!type.empty())
	{
		if (types.find(type) == types.end()) throw Exception("Unknown sound provider type " + type);

		SoundProviderType *factory = types[type];
		return factory->load(filename, streamed, fs);
	}

	// Determine file extension and use it to lookup type.
	std::string ext = PathHelp::get_extension(filename);
	if (ext.empty()) ext = filename;
	ext = StringHelp::text_to_lower(ext);
	if (types.find(ext) == types.end()) throw Exception("Unknown sound provider type " + ext);

	SoundProviderType *factory = types[ext];
	return factory->load(filename, streamed, fs);
}

SoundProvider *SoundProviderFactory::load(
	const std::string &fullname,
	bool streamed,
	const std::string &type)
{
	SetupSound::start();
	std::string path = PathHelp::get_fullpath(fullname, PathHelp::path_type_file);
	std::string filename = PathHelp::get_filename(fullname, PathHelp::path_type_file);
	FileSystem vfs(path);
	return SoundProviderFactory::load(filename, streamed, vfs, type);
}

SoundProvider *SoundProviderFactory::load(
	IODevice &file,
	bool streamed,
	const std::string &type)
{
	SetupSound::start();
	auto &types = *SetupSound::get_sound_provider_factory_types();

	if (types.find(type) == types.end()) throw Exception("Unknown sound provider type " + type);

	SoundProviderType *factory = types[type];
	return factory->load(file, streamed);
}
