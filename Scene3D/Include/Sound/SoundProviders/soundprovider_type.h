
#pragma once

class SoundProvider;

class SoundProviderType
{
public:
	SoundProviderType(const std::string &type);
	virtual ~SoundProviderType();
	virtual SoundProvider *load(const std::string &filename, bool streamed, const uicore::FileSystem &file_system) = 0;
	virtual SoundProvider *load(uicore::IODevice &file, bool streamed) = 0;
};
