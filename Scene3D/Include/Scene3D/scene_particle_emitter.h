
#pragma once

#include <memory>

class Scene;
typedef std::shared_ptr<Scene> ScenePtr;

class SceneParticleEmitter
{
public:
	static std::shared_ptr<SceneParticleEmitter> create(const ScenePtr &scene);

	enum Type
	{
		type_omni,
		type_spot
	};

	virtual Type type() const = 0;
	virtual uicore::Vec3f position() const = 0;
	virtual uicore::Quaternionf orientation() const = 0;
	virtual float particles_per_second() const = 0;
	virtual float falloff() const = 0;
	virtual float life_span() const = 0;
	virtual float start_size() const = 0;
	virtual float end_size() const = 0;
	virtual float speed() const = 0;
	virtual uicore::Vec3f acceleration() const = 0;
	virtual std::string particle_texture() const = 0;
	virtual std::string gradient_texture() const = 0;

	virtual void set_type(Type type) = 0;
	virtual void set_position(const uicore::Vec3f &position) = 0;
	virtual void set_orientation(const uicore::Quaternionf &orientation) = 0;
	virtual void set_particles_per_second(float pps) = 0;
	virtual void set_falloff(float falloff) = 0;
	virtual void set_life_span(float life_span) = 0;
	virtual void set_start_size(float size) = 0;
	virtual void set_end_size(float size) = 0;
	virtual void set_speed(float speed) = 0;
	virtual void set_acceleration(const uicore::Vec3f &acceleration) = 0;
	virtual void set_particle_texture(const std::string &texture) = 0;
	virtual void set_gradient_texture(const std::string &texture) = 0;
};

typedef std::shared_ptr<SceneParticleEmitter> SceneParticleEmitterPtr;
