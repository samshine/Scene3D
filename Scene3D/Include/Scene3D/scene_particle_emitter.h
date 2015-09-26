
#pragma once

#include <memory>

class Scene;
class SceneParticleEmitter_Impl;
class OctTreeObject;
class ParticleEmitterPassData;

class SceneParticleEmitter
{
public:
	SceneParticleEmitter();
	SceneParticleEmitter(Scene &scene);
	bool is_null() const { return !impl; }

	enum Type
	{
		type_omni,
		type_spot
	};

	Type get_type() const;
	uicore::Vec3f get_position() const;
	uicore::Quaternionf get_orientation() const;
	float get_particles_per_second() const;
	float get_falloff() const;
	float get_life_span() const;
	float get_start_size() const;
	float get_end_size() const;
	float get_speed() const;
	uicore::Vec3f get_acceleration() const;
	std::string get_particle_texture() const;
	std::string get_gradient_texture() const;

	void set_type(Type type);
	void set_position(const uicore::Vec3f &position);
	void set_orientation(const uicore::Quaternionf &orientation);
	void set_particles_per_second(float pps);
	void set_falloff(float falloff);
	void set_life_span(float life_span);
	void set_start_size(float size);
	void set_end_size(float size);
	void set_speed(float speed);
	void set_acceleration(const uicore::Vec3f &acceleration);
	void set_particle_texture(const std::string &texture);
	void set_gradient_texture(const std::string &texture);

private:
	std::shared_ptr<SceneParticleEmitter_Impl> impl;
};
