
#pragma once

#include <string>

class AudioDefinition_Impl
{
public:
	AudioDefinition_Impl();

	float attenuation_begin;
	float attenuation_end;
	float volume;
	bool looping;
	bool ambience;
	std::string sound_id;
};
