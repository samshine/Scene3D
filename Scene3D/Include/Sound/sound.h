
#pragma once

class SoundOutput;

class Sound
{
public:
	static SoundOutput get_current_output();
	static void select_output(const SoundOutput &output);
};
