
#include "precomp.h"
#include "Sound/soundoutput.h"
#include "Sound/soundoutput_description.h"
#include "Sound/sound.h"
#include "soundoutput_impl.h"
#include "setupsound.h"

#ifdef WIN32
#include "Platform/Win32/soundoutput_win32.h"
#else
#ifdef __APPLE__
#include "Platform/MacOSX/soundoutput_macosx.h"
#else
#include "Platform/Linux/soundoutput_oss.h"
#endif
#endif

#ifdef HAVE_ALSA_ASOUNDLIB_H
#include "Platform/Linux/soundoutput_alsa.h"
#endif

using namespace uicore;

SoundOutput::SoundOutput()
{
}

SoundOutput::SoundOutput(int mixing_frequency, int latency)
{
	SoundOutput_Description desc;
	desc.set_mixing_frequency(mixing_frequency);
	desc.set_mixing_latency(latency);
	operator =(SoundOutput(desc));
}

SoundOutput::SoundOutput(const SoundOutput_Description &desc)
{
	SetupSound::start();

#ifdef WIN32
	impl = std::make_shared<SoundOutput_Win32>(desc.get_mixing_frequency(), desc.get_mixing_latency());
#else
#ifdef __APPLE__
	std::shared_ptr<SoundOutputImpl> soundoutput_impl(std::make_shared<SoundOutput_MacOSX>(desc.get_mixing_frequency(), desc.get_mixing_latency()));
	impl = soundoutput_impl;
#else
#if defined(__linux__) && defined(HAVE_ALSA_ASOUNDLIB_H)
	// Try building ALSA

	std::shared_ptr<SoundOutputImpl> alsa_impl(std::make_shared<SoundOutput_alsa>(desc.get_mixing_frequency(), desc.get_mixing_latency()));
	if ( ( (SoundOutput_alsa *) (alsa_impl.get()))->handle)
	{
		impl = alsa_impl;
	}
	else
	{
		alsa_impl.reset();
	}

	if (!impl)
	{
		std::shared_ptr<SoundOutputImpl> soundoutput_impl(std::make_shared<SoundOutput_OSS>(desc.get_mixing_frequency(), desc.get_mixing_latency()));
		impl = soundoutput_impl;
	}
#else
	std::shared_ptr<SoundOutputImpl> soundoutput_impl(std::make_shared<SoundOutput_OSS>(desc.get_mixing_frequency(), desc.get_mixing_latency()));
	impl = soundoutput_impl;
#endif
#endif
#endif
	Sound::select_output(*this);
}

SoundOutput::~SoundOutput()
{
}

SoundOutput::SoundOutput(const std::weak_ptr<SoundOutputImpl> impl)
	: impl(impl.lock())
{
}

void SoundOutput::throw_if_null() const
{
	if (!impl)
		throw Exception("SoundOutput is null");
}

const std::string &SoundOutput::get_name() const
{
	std::unique_lock<std::recursive_mutex> mutex_lock(impl->mutex);
	return impl->name;
}

int SoundOutput::get_mixing_frequency() const
{
	std::unique_lock<std::recursive_mutex> mutex_lock(impl->mutex);
	return impl->mixing_frequency;
}

int SoundOutput::get_mixing_latency() const
{
	std::unique_lock<std::recursive_mutex> mutex_lock(impl->mutex);
	return impl->mixing_latency;
}

float SoundOutput::get_global_volume() const
{
	std::unique_lock<std::recursive_mutex> mutex_lock(impl->mutex);
	return impl->volume;
}

float SoundOutput::get_global_pan() const
{
	std::unique_lock<std::recursive_mutex> mutex_lock(impl->mutex);
	return impl->pan;
}

void SoundOutput::stop_all()
{
}

void SoundOutput::set_global_volume(float volume)
{
	if (impl)
	{
		std::unique_lock<std::recursive_mutex> mutex_lock(impl->mutex);
		impl->volume = volume;
	}
}

void SoundOutput::set_global_pan(float pan)
{
	if (impl)
	{
		std::unique_lock<std::recursive_mutex> mutex_lock(impl->mutex);
		impl->pan = pan;
	}
}
