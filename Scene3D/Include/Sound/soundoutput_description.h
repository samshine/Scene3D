
#pragma once

#include <memory>

class SoundOutput_Description_Impl;

class SoundOutput_Description
{
public:
	SoundOutput_Description();
	~SoundOutput_Description();

	int get_mixing_frequency() const;
	int get_mixing_latency() const;
	void set_mixing_frequency(int frequency);
	void set_mixing_latency(int latency);

private:
	std::shared_ptr<SoundOutput_Description_Impl> impl;
};
