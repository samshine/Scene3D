
#pragma once

class SoundMixingBuffersData
{
public:
	SoundMixingBuffersData() { for (auto & elem : channels) elem = nullptr; }

	float *channels[32];
};
