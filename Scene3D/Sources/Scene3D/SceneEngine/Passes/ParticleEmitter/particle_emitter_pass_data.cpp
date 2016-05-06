
#include "precomp.h"
#include "particle_emitter_pass_data.h"
#include "Scene3D/Scene/scene_particle_emitter_impl.h"

using namespace uicore;

void ParticleEmitterPassData::reset()
{
	time_to_next_emit = 0.0f;
	cpu_particles.clear();
	last_emitted_index = 0;
	emitter = nullptr;
	life_color_gradient = Resource<uicore::TexturePtr>();
	particle_animation = Resource<uicore::TexturePtr>();
}

bool ParticleEmitterPassData::update(float time_elapsed)
{
	if (visible)
	{
		size_t particles_required = (size_t)(emitter->life_span() * emitter->particles_per_second()) + 8;
		if (cpu_particles.size() < particles_required)
			cpu_particles.resize(particles_required);

		time_to_next_emit -= time_elapsed;
		if (time_to_next_emit <= 0.0f)
		{
			emit();
			time_to_next_emit = 1.0f / emitter->particles_per_second();
		}
	}

	bool particles_active = advance(time_elapsed);
	return visible || particles_active;
}

bool ParticleEmitterPassData::advance(float time_elapsed)
{
	bool active = false;
	for (size_t index = 0; index < cpu_particles.size(); index++)
	{
		if (cpu_particles[index].life > -1.0f)
		{
			cpu_particles[index].velocity += cpu_particles[index].acceleration * time_elapsed;
			cpu_particles[index].position += cpu_particles[index].velocity * time_elapsed;
			cpu_particles[index].life += cpu_particles[index].life_speed * time_elapsed;

			if (cpu_particles[index].life > 1.0f)
				cpu_particles[index].life = -1.0f;
			else
				active = true;
		}
	}
	return active;
}

void ParticleEmitterPassData::emit()
{
	Quaternionf particle_orientation;
	if (emitter->type() == SceneParticleEmitter::type_spot)
		particle_orientation = emitter->orientation() * Quaternionf::euler(radians(random(-emitter->falloff(), emitter->falloff())), radians(random(-emitter->falloff(), emitter->falloff())), 0.0f);
	else
		particle_orientation = Quaternionf::euler(radians(random(-180.0f, 180.0f)), radians(random(-180.0f, 180.0f)), radians(random(-180.0f, 180.0f)), EulerOrder::zyx);

	last_emitted_index = (last_emitted_index + 1) % cpu_particles.size();
	cpu_particles[last_emitted_index].position = emitter->position();
	cpu_particles[last_emitted_index].velocity = particle_orientation.rotate_vector(Vec3f(0.0f, 0.0f, 1.0f)) * emitter->speed();
	cpu_particles[last_emitted_index].acceleration = emitter->acceleration();
	cpu_particles[last_emitted_index].life = 0.0f;
	cpu_particles[last_emitted_index].life_speed = 1.0f / emitter->life_span();
	cpu_particles[last_emitted_index].start_size = emitter->start_size();
	cpu_particles[last_emitted_index].end_size = emitter->end_size();
}

float ParticleEmitterPassData::random(float min_val, float max_val)
{
	float v = rand() / (float)RAND_MAX;
	return min_val + v * (max_val - min_val);
}
