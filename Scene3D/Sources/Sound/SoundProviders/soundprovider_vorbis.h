
#pragma once

#include "Sound/soundprovider.h"

class SoundProvider_Vorbis : public SoundProvider, public std::enable_shared_from_this<SoundProvider_Vorbis>
{
public:
	SoundProvider_Vorbis(const uicore::IODevicePtr &input, bool stream);

	std::shared_ptr<SoundProvider_Session> begin_session() override;

	uicore::DataBufferPtr buffer;
};
