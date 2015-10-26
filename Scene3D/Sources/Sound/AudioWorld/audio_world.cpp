
#include "precomp.h"
#include "Sound/AudioWorld/audio_world.h"
#include "Sound/AudioWorld/audio_object.h"
#include "Sound/soundbuffer.h"
#include "audio_world_impl.h"
#include "audio_object_impl.h"

using namespace uicore;

std::shared_ptr<AudioWorld> AudioWorld::create(const std::shared_ptr<SoundCache> &sound_cache)
{
	return std::make_shared<AudioWorldImpl>(sound_cache);
}

/////////////////////////////////////////////////////////////////////////////

AudioWorldImpl::AudioWorldImpl(const std::shared_ptr<SoundCache> &sound_cache) : sound_cache(sound_cache)
{
}

void AudioWorldImpl::set_listener(const Vec3f &position, const Quaternionf &orientation)
{
	_listener_position = position;
	_listener_orientation = orientation;
}

void AudioWorldImpl::set_ambience_enabled(bool enable)
{
	_play_ambience = enable;
}

bool AudioWorldImpl::ambience_enabled() const
{
	return _play_ambience;
}

void AudioWorldImpl::set_reverse_stereo(bool enable)
{
	_reverse_stereo = enable;
}

bool AudioWorldImpl::reverse_stereo() const
{
	return _reverse_stereo;
}

void AudioWorldImpl::update()
{
	for (auto it = objects.begin(); it != objects.end(); ++it)
	{
		update_session(*it);
	}

	for (auto it = active_objects.begin(); it != active_objects.end();)
	{
		if (static_cast<AudioObjectImpl*>((*it).get())->_session.is_playing())
		{
			++it;
		}
		else
		{
			it = active_objects.erase(it);
		}
	}
}

void AudioWorldImpl::update_session(AudioObjectImpl *obj)
{
	if (obj->_attenuation_begin != obj->_attenuation_end)
	{
		// Calculate volume from distance
		float distance = (obj->_position - _listener_position).length();
		float t = 1.0f - clamp((distance - obj->_attenuation_begin) / (obj->_attenuation_end - obj->_attenuation_begin), 0.0f, 1.0f);
		t = t * t;

		// Calculate pan from ear angle
		Vec3f sound_direction = Vec3f::normalize(obj->_position - _listener_position);
		Vec3f ear_vector = _listener_orientation.rotate_vector(Vec3f(1.0f, 0.0f, 0.0f));
		float pan = Vec3f::dot(ear_vector, sound_direction);
		if (_reverse_stereo)
			pan = -pan;

		// Final volume needs to stay the same no matter the panning direction
		float volume = (0.5f + std::abs(pan) * 0.5f) * t * obj->_volume;

		obj->_session.set_volume(volume);
		obj->_session.set_pan(pan);
	}
	else
	{
		obj->_session.set_volume(obj->_volume);
		obj->_session.set_pan(0.0f);
	}
}
