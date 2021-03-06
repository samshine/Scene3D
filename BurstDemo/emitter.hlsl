
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
		for (int i = 0; i < 100; i++)
		{
			// Create a particle
			Particle particle;
			particle.pos = emitters[index].pos;
			particle.velocity.x = cos(radians(emitters[index].emit_position * 5));
			particle.velocity.y = sin(radians(emitters[index].emit_position * 5));
			particle.velocity.z = 0;
			particle.velocity *= 20;
			particle.life = 10;
			particle.size = 0.2;
			particle.particle_subarray_start = 0;
			particle.particle_subarray_size = 0;
			particle.emit_position = 0;
			particle.emit_cooldown = 0;

			// Insert it into particles array:
			particles[emitters[index].particle_subarray_start + emitters[index].emit_position] = particle;
			emitters[index].emit_position = (emitters[index].emit_position + 1) % emitters[index].particle_subarray_size;
			emitters[index].emit_cooldown = 0.001;
		}
	}
}
