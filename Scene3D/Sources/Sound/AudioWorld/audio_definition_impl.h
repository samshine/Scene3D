
#pragma once

#include <string>

class AudioDefinitionImpl
{
public:
	AudioDefinitionImpl();

	float attenuation_begin;
	float attenuation_end;
	float volume;
	bool looping;
	bool ambience;
	std::string sound_id;
};
