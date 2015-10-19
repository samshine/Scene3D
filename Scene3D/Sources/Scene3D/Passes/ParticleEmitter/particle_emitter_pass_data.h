
#pragma once

#include "Scene3D/scene_particle_emitter_impl.h"
#include "Scene3D/SceneEngine/resource.h"
#include "particle.h"
#include "particle_uniforms.h"

class ParticleEmitterPassData
{
public:
	void reset();
	bool update(float time_elapsed);

	bool visible = false;

	float time_to_next_emit = 0.0f;
	std::vector<Particle> cpu_particles;
	int last_emitted_index = 0;

	SceneParticleEmitterImpl *emitter = nullptr;

	Resource<uicore::TexturePtr> life_color_gradient;
	Resource<uicore::TexturePtr> particle_animation;
	uicore::UniformVector<ParticleUniforms> gpu_uniforms;

private:
	bool advance(float time_elapsed);
	void emit();
	static float random(float min_val, float max_val);
};
