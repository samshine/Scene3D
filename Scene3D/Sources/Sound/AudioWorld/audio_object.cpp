
#include "precomp.h"
#include "Sound/AudioWorld/audio_object.h"
#include "Sound/AudioWorld/audio_world.h"
#include "Sound/sound_cache.h"
#include "audio_object_impl.h"
#include "audio_world_impl.h"

using namespace uicore;

std::shared_ptr<AudioObject> AudioObject::create(const AudioWorldPtr &world)
{
	return std::make_shared<AudioObjectImpl>(static_cast<AudioWorldImpl*>(world.get()));
}

/////////////////////////////////////////////////////////////////////////////

AudioObjectImpl::AudioObjectImpl(AudioWorldImpl *world) : _world(world)
{
	_it = world->objects.insert(world->objects.end(), this);
}

AudioObjectImpl::~AudioObjectImpl()
{
	_world->objects.erase(_it);
}

Vec3f AudioObjectImpl::position() const
{
	return _position;
}

float AudioObjectImpl::attenuation_begin() const
{
	return _attenuation_begin;
}

float AudioObjectImpl::attenuation_end() const
{
	return _attenuation_end;
}

float AudioObjectImpl::volume() const
{
	return _volume;
}

bool AudioObjectImpl::looping() const
{
	return _looping;
}

bool AudioObjectImpl::ambience() const
{
	return _ambience;
}

bool AudioObjectImpl::playing() const
{
	return !_session.is_null() && _session.is_playing();
}

void AudioObjectImpl::set_position(const Vec3f &position)
{
	_position = position;
}

void AudioObjectImpl::set_attenuation_begin(float distance)
{
	_attenuation_begin = distance;
}

void AudioObjectImpl::set_attenuation_end(float distance)
{
	_attenuation_end = distance;
}

void AudioObjectImpl::set_volume(float volume)
{
	_volume = volume;
}

void AudioObjectImpl::set_sound(const SoundBuffer &buffer)
{
	_sound = buffer;
}

void AudioObjectImpl::set_sound(const std::string &id)
{
	_sound = _world->sound_cache->get(id);
}

void AudioObjectImpl::set_looping(bool loop)
{
	_looping = loop;
}

void AudioObjectImpl::set_ambience(bool ambience)
{
	_ambience = ambience;
}

void AudioObjectImpl::play()
{
	if (!_ambience || _world->ambience_enabled())
	{
		_session = _sound.prepare(_looping);
		_world->update_session(this);
		_session.play();
		_world->active_objects.push_back(shared_from_this());
	}
}

void AudioObjectImpl::stop()
{
	if (!_session.is_null())
	{
		_session.stop();
		_session = SoundBuffer_Session();
	}
}
