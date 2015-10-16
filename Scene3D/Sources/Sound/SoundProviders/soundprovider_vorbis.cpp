
#include "precomp.h"
#include "Sound/SoundProviders/soundprovider_vorbis.h"
#include "soundprovider_vorbis_impl.h"
#include "soundprovider_vorbis_session.h"

using namespace uicore;

SoundProvider_Vorbis::SoundProvider_Vorbis(const std::string &filename, bool stream) : impl(std::make_shared<SoundProvider_VorbisImpl>())
{
	auto input = File::open_existing(filename);
	impl->load(*input);
}

SoundProvider_Vorbis::SoundProvider_Vorbis(IODevice &file, bool stream) : impl(std::make_shared<SoundProvider_VorbisImpl>())
{
	impl->load(file);
}

SoundProvider_Vorbis::~SoundProvider_Vorbis()
{
}

SoundProvider_Session *SoundProvider_Vorbis::begin_session()
{
	return new SoundProvider_Vorbis_Session(*this);
}

void SoundProvider_Vorbis::end_session(SoundProvider_Session *session)
{
	delete session;
}

void SoundProvider_VorbisImpl::load(IODevice &input)
{
	int size = input.size();
	buffer = DataBuffer::create(size);
	input.read(buffer->data(), buffer->size());
}
