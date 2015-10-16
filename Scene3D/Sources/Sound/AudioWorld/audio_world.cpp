
#include "precomp.h"
#include "Sound/AudioWorld/audio_world.h"
#include "Sound/AudioWorld/audio_object.h"
#include "Sound/soundbuffer.h"
#include "audio_world_impl.h"
#include "audio_object_impl.h"

using namespace uicore;

AudioWorld::AudioWorld(const std::shared_ptr<SoundCache> &sound_cache)
	: impl(std::make_shared<AudioWorldImpl>(sound_cache))
{
}

void AudioWorld::set_listener(const Vec3f &position, const Quaternionf &orientation)
{
	impl->listener_position = position;
	impl->listener_orientation = orientation;
}

bool AudioWorld::is_ambience_enabled() const
{
	return impl->play_ambience;
}

void AudioWorld::enable_reverse_stereo(bool enable)
{
	impl->reverse_stereo = enable;
}

bool AudioWorld::is_reverse_stereo_enabled() const
{
	return impl->reverse_stereo;
}

void AudioWorld::update()
{
	for (auto it = impl->objects.begin(); it != impl->objects.end(); ++it)
	{
		impl->update_session(*it);
	}

	for (auto it = impl->active_objects.begin(); it != impl->active_objects.end();)
	{
		if (it->impl->session.is_playing())
		{
			++it;
		}
		else
		{
			it = impl->active_objects.erase(it);
		}
	}
}

/////////////////////////////////////////////////////////////////////////////

AudioWorldImpl::AudioWorldImpl(const std::shared_ptr<SoundCache> &sound_cache) : sound_cache(sound_cache)
{
}

AudioWorldImpl::~AudioWorldImpl()
{
}

void AudioWorldImpl::update_session(AudioObjectImpl *obj)
{
	if (obj->attenuation_begin != obj->attenuation_end)
	{
		// Calculate volume from distance
		float distance = obj->position.distance(listener_position);
		float t = 1.0f - smoothstep(obj->attenuation_begin, obj->attenuation_end, distance);

		// Calculate pan from ear angle
		Vec3f sound_direction = Vec3f::normalize(obj->position - listener_position);
		Vec3f ear_vector = listener_orientation.rotate_vector(Vec3f(1.0f, 0.0f, 0.0f));
		float pan = Vec3f::dot(ear_vector, sound_direction);
		if (reverse_stereo)
			pan = -pan;

		// Final volume needs to stay the same no matter the panning direction
		float volume = (0.5f + std::abs(pan) * 0.5f) * t * obj->volume;

		obj->session.set_volume(volume);
		obj->session.set_pan(pan);
	}
	else
	{
		obj->session.set_volume(obj->volume);
		obj->session.set_pan(0.0f);
	}
}
