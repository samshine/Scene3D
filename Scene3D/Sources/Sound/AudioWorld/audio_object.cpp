
#include "precomp.h"
#include "Sound/AudioWorld/audio_object.h"
#include "Sound/AudioWorld/audio_world.h"
#include "Sound/sound_cache.h"
#include "audio_object_impl.h"
#include "audio_world_impl.h"

using namespace uicore;

AudioObject::AudioObject()
{
}

AudioObject::AudioObject(AudioWorld &world)
	: impl(std::make_shared<AudioObjectImpl>(world.impl.get()))
{
}

AudioObject::AudioObject(AudioWorld &world, AudioDefinition definition)
	: impl(std::make_shared<AudioObjectImpl>(world.impl.get()))
{
	set_attenuation_begin(definition.get_attenuation_begin());
	set_attenuation_end(definition.get_attenuation_end());
	set_volume(definition.get_volume());
	set_sound(definition.get_sound_id());
	set_looping(definition.is_looping());
	set_ambience(definition.is_ambience());
}

Vec3f AudioObject::get_position() const
{
	return impl->position;
}

float AudioObject::get_attenuation_begin() const
{
	return impl->attenuation_begin;
}

float AudioObject::get_attenuation_end() const
{
	return impl->attenuation_end;
}

float AudioObject::get_volume() const
{
	return impl->volume;
}

bool AudioObject::is_looping() const
{
	return impl->looping;
}

bool AudioObject::is_ambience() const
{
	return impl->ambience;
}

bool AudioObject::is_playing() const
{
	return impl && !impl->session.is_null() && impl->session.is_playing();
}

void AudioObject::set_position(const Vec3f &position)
{
	impl->position = position;
}

void AudioObject::set_attenuation_begin(float distance)
{
	impl->attenuation_begin = distance;
}

void AudioObject::set_attenuation_end(float distance)
{
	impl->attenuation_end = distance;
}

void AudioObject::set_volume(float volume)
{
	impl->volume = volume;
}

void AudioObject::set_sound(const SoundBuffer &buffer)
{
	impl->sound = buffer;
}

void AudioObject::set_sound(const std::string &id)
{
	impl->sound = impl->world->sound_cache->get(id);
}

void AudioObject::set_looping(bool loop)
{
	impl->looping = loop;
}

void AudioObject::set_ambience(bool ambience)
{
	impl->ambience = ambience;
}

void AudioObject::play()
{
	if (!impl->ambience || impl->world->play_ambience)
	{
		impl->session = impl->sound.prepare(impl->looping);
		impl->world->update_session(impl.get());
		impl->session.play();
		impl->world->active_objects.push_back(*this);
	}
}

void AudioObject::stop()
{
	if (impl && !impl->session.is_null())
	{
		impl->session.stop();
		impl->session = SoundBuffer_Session();
	}
}

/////////////////////////////////////////////////////////////////////////////

AudioObjectImpl::AudioObjectImpl(AudioWorldImpl *world)
	: world(world), attenuation_begin(0.0f), attenuation_end(0.0f), volume(1.0f), looping(false), ambience(false)
{
	it = world->objects.insert(world->objects.end(), this);
}

AudioObjectImpl::~AudioObjectImpl()
{
	world->objects.erase(it);
}
