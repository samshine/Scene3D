
#pragma once

#include <memory>

class AudioDefinition_Impl;

class AudioDefinition
{
public:
	AudioDefinition();

	bool is_null() const { return !impl; }

	std::string get_sound_id() const;

	float get_attenuation_begin() const;
	float get_attenuation_end() const;
	float get_volume() const;

	bool is_looping() const;
	bool is_ambience() const;

	void set_sound_id(const std::string &id);

	void set_attenuation_begin(float distance);
	void set_attenuation_end(float distance);
	void set_volume(float volume);

	void set_looping(bool loop);
	void set_ambience(bool ambience);

private:
	std::shared_ptr<AudioDefinition_Impl> impl;
};
