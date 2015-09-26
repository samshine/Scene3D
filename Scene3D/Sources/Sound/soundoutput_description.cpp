
#include "precomp.h"
#include "Sound/soundoutput_description.h"

class SoundOutput_Description_Impl
{
public:
	int mixing_frequency;
	int mixing_latency;
};

SoundOutput_Description::SoundOutput_Description() : impl(std::make_shared<SoundOutput_Description_Impl>())
{
	impl->mixing_frequency = 44100;
	impl->mixing_latency = 50;
}

SoundOutput_Description::~SoundOutput_Description()
{
}

int SoundOutput_Description::get_mixing_frequency() const
{
	return impl->mixing_frequency;
}

int SoundOutput_Description::get_mixing_latency() const
{
	return impl->mixing_latency;
}

void SoundOutput_Description::set_mixing_frequency(int frequency)
{
	impl->mixing_frequency = frequency;
}

void SoundOutput_Description::set_mixing_latency(int latency)
{
	impl->mixing_latency = latency;
}
