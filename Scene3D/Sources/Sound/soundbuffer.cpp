/*
**  ClanLib SDK
**  Copyright (c) 1997-2015 The ClanLib Team
**
**  This software is provided 'as-is', without any express or implied
**  warranty.  In no event will the authors be held liable for any damages
**  arising from the use of this software.
**
**  Permission is granted to anyone to use this software for any purpose,
**  including commercial applications, and to alter it and redistribute it
**  freely, subject to the following restrictions:
**
**  1. The origin of this software must not be misrepresented; you must not
**     claim that you wrote the original software. If you use this software
**     in a product, an acknowledgment in the product documentation would be
**     appreciated but is not required.
**  2. Altered source versions must be plainly marked as such, and must not be
**     misrepresented as being the original software.
**  3. This notice may not be removed or altered from any source distribution.
**
**  Note: Some of the libraries ClanLib may link to may have additional
**  requirements or restrictions.
**
**  File Author(s):
**
**    Magnus Norddahl
*/

#include "precomp.h"
#include "Sound/soundbuffer.h"
#include "Sound/soundbuffer_session.h"
#include "Sound/soundoutput.h"
#include "Sound/sound.h"
#include "Sound/SoundProviders/soundprovider_factory.h"
#include "soundbuffer_impl.h"
#include "soundbuffer_session_impl.h"

namespace uicore
{
	SoundBuffer::SoundBuffer()
	{
	}

	SoundBuffer::SoundBuffer(
		SoundProvider *provider)
		: impl(std::make_shared<SoundBuffer_Impl>())
	{
		impl->provider = provider;
	}

	SoundBuffer::SoundBuffer(
		const std::string &fullname,
		bool streamed,
		const std::string &sound_format)
		: impl(std::make_shared<SoundBuffer_Impl>())
	{
		impl->provider = SoundProviderFactory::load(fullname, streamed, sound_format);
	}

	SoundBuffer::SoundBuffer(
		const std::string &filename,
		bool streamed,
		const FileSystem &fs,
		const std::string &type)
		: impl(std::make_shared<SoundBuffer_Impl>())
	{
		impl->provider = SoundProviderFactory::load(filename, streamed, fs, type);
	}

	SoundBuffer::SoundBuffer(
		IODevice &file,
		bool streamed,
		const std::string &type)
		: impl(std::make_shared<SoundBuffer_Impl>())
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
}
