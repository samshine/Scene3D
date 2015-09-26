
#include "precomp.h"

#ifdef _MSC_VER
#pragma warning(disable : 4786)
#endif

#include "Sound/SoundProviders/soundprovider_type.h"
#include "Sound/SoundProviders/soundprovider_factory.h"
#include "../setupsound.h"

SoundProviderType::SoundProviderType(const std::string &type)
{
	auto &types = *SetupSound::get_sound_provider_factory_types();
	types[type] = this;
}

SoundProviderType::~SoundProviderType()
{
	auto &types = *SetupSound::get_sound_provider_factory_types();
	std::map<std::string, SoundProviderType *>::iterator it;

	for (it = types.begin(); it != types.end(); it++)
	{
		if (it->second == this)
		{
			types.erase(it);
			break;
		}
	}
}
