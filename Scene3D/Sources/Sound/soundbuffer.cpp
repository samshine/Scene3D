
#include "precomp.h"
#include "Sound/soundbuffer.h"
#include "Sound/soundbuffer_session.h"
#include "Sound/soundoutput.h"
#include "Sound/sound.h"
#include "Sound/SoundProviders/soundprovider_factory.h"
#include "soundbuffer_impl.h"
#include "soundbuffer_session_impl.h"

using namespace uicore;

SoundBuffer::SoundBuffer()
{
}

SoundBuffer::SoundBuffer(SoundProvider *provider) : impl(std::make_shared<SoundBuffer_Impl>())
{
	impl->provider = provider;
}

SoundBuffer::SoundBuffer(const std::string &fullname, bool streamed, const std::string &sound_format) : impl(std::make_shared<SoundBuffer_Impl>())
{
	impl->provider = SoundProviderFactory::load(fullname, streamed, sound_format);
}

SoundBuffer::SoundBuffer(IODevice &file, bool streamed, const std::string &type) : impl(std::make_shared<SoundBuffer_Impl>())
{
	impl->provider = SoundProviderFactory::load(file, streamed, type);
}

SoundBuffer::~SoundBuffer()
{
}

SoundProvider *SoundBuffer::get_provider() const
{
	if (!impl)
		return nullptr;
	return impl->provider;
}

void SoundBuffer::throw_if_null() const
{
	if (!impl)
		throw Exception("SoundBuffer is null");
}

float SoundBuffer::get_volume() const
{
	return impl->volume;
}

float SoundBuffer::get_pan() const
{
	return impl->pan;
}

void SoundBuffer::set_volume(float new_volume)
{
	std::unique_lock<std::recursive_mutex> mutex_lock(impl->mutex);
	impl->volume = new_volume;
}

void SoundBuffer::set_pan(float new_pan)
{
	std::unique_lock<std::recursive_mutex> mutex_lock(impl->mutex);
	impl->pan = new_pan;
}

SoundBuffer_Session SoundBuffer::play(bool looping, SoundOutput *output)
{
	SoundBuffer_Session session = prepare(looping, output);
	session.play();
	return session;
}

SoundBuffer_Session SoundBuffer::prepare(bool looping, SoundOutput *output)
{
	SoundOutput current_output = Sound::get_current_output();
	if (output == nullptr) output = &current_output;

	std::unique_lock<std::recursive_mutex> mutex_lock(impl->mutex);
	return SoundBuffer_Session(*this, looping, *output);
}
