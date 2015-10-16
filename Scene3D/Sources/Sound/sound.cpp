
#include "precomp.h"
#include "Sound/sound.h"
#include "Sound/soundoutput.h"
#include <memory>

static std::weak_ptr<SoundOutputImpl> cl_current_output;

SoundOutput Sound::get_current_output()
{
	return SoundOutput(cl_current_output);
}

void Sound::select_output(const SoundOutput &output)
{
	cl_current_output = output.impl;
}
