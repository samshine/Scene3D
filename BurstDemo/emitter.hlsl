
struct Particle
{
	float3 pos;
	float size;
	float3 velocity;
	float life;

	uint particle_subarray_start;
	uint particle_subarray_size;
	uint emit_position;
	float emit_cooldown;
};

RWStructuredBuffer<Particle> emitters;
RWStructuredBuffer<Particle> particles;

[numthreads(1, 1, 1)]
void main(uint3 threadId : SV_DispatchThreadID)
{
	uint index = threadId.x;

	float time_elapsed = 0.016666f;

	emitters[index].emit_cooldown -= time_elapsed;

	// Is it time to emit?
	if (emitters[index].emit_cooldown <= 0 && emitters[index].life > 0)
	{
		// Create a particle
		Particle particle;
		particle.pos = emitters[index].pos;
		particle.velocity.x = cos(radians(index)) * 2 + index / 400.0;
		particle.velocity.y = cos(radians(index)) * 2 + index / 400.0;
		particle.velocity.z = 0;
		particle.life = 20;
		particle.size = 1 - index / float(1024) + 0.001;
		particle.particle_subarray_start = 0;
		particle.particle_subarray_size = 0;
		particle.emit_position = 0;
		particle.emit_cooldown = 0;

		// Insert it into particles array:
		particles[emitters[index].particle_subarray_start + emitters[index].emit_position] = particle;
		emitters[index].emit_position = (emitters[index].emit_position + 1) % emitters[index].particle_subarray_size;
		emitters[index].emit_cooldown = 2;
	}
}
