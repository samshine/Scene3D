
#include "precomp.h"
#include "soundprovider_vorbis.h"
#include "soundprovider_vorbis_session.h"

using namespace uicore;

SoundProvider_Vorbis::SoundProvider_Vorbis(const IODevicePtr &input, bool stream)
{
	int size = input->size();
	buffer = DataBuffer::create(size);
	input->read(buffer->data(), buffer->size());
}

std::shared_ptr<SoundProvider_Session> SoundProvider_Vorbis::begin_session()
{
	return std::make_shared<SoundProvider_Vorbis_Session>(shared_from_this());
}
