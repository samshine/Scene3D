
#pragma once

#include "Sound/soundformat.h"
#include <string>

class SoundProvider_Vorbis_Impl
{
public:
	void load(uicore::IODevice &input);

	uicore::DataBuffer buffer;
};
