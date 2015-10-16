
#pragma once

#include "Sound/soundformat.h"
#include <string>

class SoundProvider_VorbisImpl
{
public:
	void load(uicore::IODevice &input);

	uicore::DataBufferPtr buffer;
};
