
#pragma once

#include "soundprovider_type.h"

class SoundProvider;

template<class SoundProviderClass>
class SoundProviderType_Register : public SoundProviderType
{
public:
	SoundProviderType_Register(const std::string &type) : SoundProviderType(type) { }

	virtual SoundProvider *load(const std::string &filename, bool stream, const uicore::FileSystem &fs) override
	{
		return new SoundProviderClass(filename, fs, stream);
	}

	virtual SoundProvider *load(uicore::IODevice &file, bool stream) override
	{
		return new SoundProviderClass(file, stream);
	}
};
