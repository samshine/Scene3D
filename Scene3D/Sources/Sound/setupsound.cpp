
#include "precomp.h"
#include "setupsound.h"
#include "Sound/SoundProviders/soundprovider_wave.h"
#include "Sound/SoundProviders/soundprovider_vorbis.h"
#include "Sound/SoundProviders/soundprovider_type_register.h"

#define INCLUDED_FROM_SETUPVORBIS
#include "SoundProviders/stb_vorbis.h"

void SetupSound::start()
{
	static bool first_time = true;
	if (first_time)
	{
		first_time = false;
		static auto providertype_wave = std::make_unique<SoundProviderType_Register<SoundProvider_Wave>>("wav");
		static auto providertype_ogg = std::make_unique<SoundProviderType_Register<SoundProvider_Vorbis>>("ogg");
	}
}

std::map<std::string, SoundProviderType *> *SetupSound::get_sound_provider_factory_types()
{
	static std::map<std::string, SoundProviderType *> types;
	return &types;
}

