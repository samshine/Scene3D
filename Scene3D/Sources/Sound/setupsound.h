
#pragma once

#include <map>

class SoundProviderType;

class SetupSound
{
public:
	static void start();
	static std::map<std::string, SoundProviderType *> *get_sound_provider_factory_types();
};
