
#include "precomp.h"
#include "Scene3D/scene_particle_emitter.h"
#include "Scene3D/scene.h"
#include "Passes/ParticleEmitter/particle_emitter_pass_data.h"
#include "Scene3D/scene_particle_emitter_impl.h"
#include "Scene3D/scene_impl.h"

using namespace uicore;

SceneParticleEmitter::SceneParticleEmitter()
{
}

SceneParticleEmitter::SceneParticleEmitter(Scene &scene)
	: impl(std::make_shared<SceneParticleEmitter_Impl>(scene.impl.get()))
{
	impl->cull_proxy = impl->scene->cull_provider->create_proxy(impl.get(), impl->get_aabb());
}

SceneParticleEmitter::Type SceneParticleEmitter::get_type() const
{
	return impl->type;
}

Vec3f SceneParticleEmitter::get_position() const
{
	return impl->position;
}

Quaternionf SceneParticleEmitter::get_orientation() const
{
	return impl->orientation;
}

float SceneParticleEmitter::get_particles_per_second() const
{
	return impl->particles_per_second;
}

float SceneParticleEmitter::get_falloff() const
{
	return impl->falloff;
}

float SceneParticleEmitter::get_life_span() const
{
	return impl->life_span;
}

float SceneParticleEmitter::get_start_size() const
{
	return impl->start_size;
}

float SceneParticleEmitter::get_end_size() const
{
	return impl->end_size;
}

float SceneParticleEmitter::get_speed() const
{
	return impl->speed;
}

Vec3f SceneParticleEmitter::get_acceleration() const
{
	return impl->acceleration;
}

std::string SceneParticleEmitter::get_particle_texture() const
{
	return impl->particle_texture;
}

std::string SceneParticleEmitter::get_gradient_texture() const
{
	return impl->gradient_texture;
}

void SceneParticleEmitter::set_type(Type type)
{
	impl->type = type;
}

void SceneParticleEmitter::set_position(const Vec3f &position)
{
	if (impl->position != position)
	{
		impl->position = position;
		if (impl->cull_proxy)
			impl->scene->cull_provider->set_aabb(impl->cull_proxy, impl->get_aabb());
	}
}

void SceneParticleEmitter::set_orientation(const Quaternionf &orientation)
{
	impl->orientation = orientation;
}

void SceneParticleEmitter::set_particles_per_second(float pps)
{
	impl->particles_per_second = pps;
}

void SceneParticleEmitter::set_falloff(float falloff)
{
	impl->falloff = falloff;
}

void SceneParticleEmitter::set_life_span(float life_span)
{
	impl->life_span = life_span;
}

void SceneParticleEmitter::set_start_size(float size)
{
	impl->start_size = size;
}

void SceneParticleEmitter::set_end_size(float size)
{
	impl->end_size = size;
}

void SceneParticleEmitter::set_speed(float speed)
{
	impl->speed = speed;
}

void SceneParticleEmitter::set_acceleration(const Vec3f &acceleration)
{
	impl->acceleration = acceleration;
}

void SceneParticleEmitter::set_particle_texture(const std::string &texture)
{
	impl->particle_texture = texture;
}

void SceneParticleEmitter::set_gradient_texture(const std::string &texture)
{
	impl->gradient_texture = texture;
}

/////////////////////////////////////////////////////////////////////////////

SceneParticleEmitter_Impl::SceneParticleEmitter_Impl(Scene_Impl *scene)
: scene(scene), cull_proxy(0), type(SceneParticleEmitter::type_omni), particles_per_second(10), falloff(90.0f), life_span(5.0f), start_size(1.0f), end_size(2.0f), speed(10.0f)
{
	it = scene->emitters.insert(scene->emitters.end(), this);
}

SceneParticleEmitter_Impl::~SceneParticleEmitter_Impl()
{
	if (cull_proxy)
		scene->cull_provider->delete_proxy(cull_proxy);
	scene->emitters.erase(it);

	if (pass_data)
	{
		pass_data->visible = false;
		pass_data->emitter = 0;
	}
}

AxisAlignedBoundingBox SceneParticleEmitter_Impl::get_aabb()
{
	float attenuation_end = 30.0f;
	AxisAlignedBoundingBox aabb;
	aabb.aabb_min = position - attenuation_end * 1.73205081f;
	aabb.aabb_max = position + attenuation_end * 1.73205081f;
	return aabb;
}
