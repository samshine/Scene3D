
#pragma once

#include "Scene3D/scene_particle_emitter.h"
#include "Scene3D/Culling/scene_cull_provider.h"
#include <list>

class ParticleEmitterPassData;
class SceneImpl;

class SceneParticleEmitterImpl : public SceneParticleEmitter, public SceneItem
{
public:
	SceneParticleEmitterImpl(SceneImpl *scene);
	~SceneParticleEmitterImpl();

	Type type() const override { return _type; }
	uicore::Vec3f position() const override { return _position; }
	uicore::Quaternionf orientation() const override { return _orientation; }
	float particles_per_second() const override { return _particles_per_second; }
	float falloff() const override { return _falloff; }
	float life_span() const override { return _life_span; }
	float start_size() const override { return _start_size; }
	float end_size() const override { return _end_size; }
	float speed() const override { return _speed; }
	uicore::Vec3f acceleration() const override { return _acceleration; }
	std::string particle_texture() const override { return _particle_texture; }
	std::string gradient_texture() const override { return _gradient_texture; }

	void set_type(Type type) override { _type = type; }
	void set_position(const uicore::Vec3f &position) override;
	void set_orientation(const uicore::Quaternionf &orientation) override;
	void set_particles_per_second(float pps) override { _particles_per_second = pps; }
	void set_falloff(float falloff) override { _falloff = falloff; }
	void set_life_span(float life_span) override { _life_span = life_span; }
	void set_start_size(float size) override { _start_size = size; }
	void set_end_size(float size) override { _end_size = size; }
	void set_speed(float speed) override { _speed = speed; }
	void set_acceleration(const uicore::Vec3f &acceleration) override { _acceleration = acceleration; }
	void set_particle_texture(const std::string &texture) override { _particle_texture = texture; }
	void set_gradient_texture(const std::string &texture) override { _gradient_texture = texture; }

	uicore::AxisAlignedBoundingBox get_aabb();

	SceneImpl *scene = nullptr;
	SceneCullProxy *cull_proxy = nullptr;
	std::list<SceneParticleEmitterImpl *>::iterator it;

	SceneParticleEmitter::Type _type = SceneParticleEmitter::type_omni;
	uicore::Vec3f _position;
	uicore::Quaternionf _orientation;

	float _particles_per_second = 10;
	float _falloff = 90.0f;
	float _life_span = 5.0f;
	float _start_size = 1.0f;
	float _end_size = 2.0f;
	float _speed = 10.0f;
	uicore::Vec3f _acceleration;
	std::string _particle_texture;
	std::string _gradient_texture;

	ParticleEmitterPassData *pass_data = nullptr;
};
