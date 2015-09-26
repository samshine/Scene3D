
#pragma once

class SoundProvider;

class SoundProviderFactory
{
public:
	static SoundProvider *load(const std::string &filename, bool streamed, const uicore::FileSystem &fs, const std::string &type = std::string());
	static SoundProvider *load(const std::string &fullname, bool streamed, const std::string &type = std::string());
	static SoundProvider *load(uicore::IODevice &file, bool streamed, const std::string &type);
};
