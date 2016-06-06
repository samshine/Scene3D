
struct Particle
{
	float3 pos;
	float size;
	float3 velocity;
	float life;

	int particle_subarray_start;
	int particle_subarray_size;
	int emit_position;
	float emit_cooldown;
};

RWStructuredBuffer<Particle> particles;

[numthreads(1, 1, 1)]
void main(uint3 threadId : SV_DispatchThreadID )
{
	uint index = threadId.x;

	float time_elapsed = 0.016666f;

	particles[index].pos += particles[index].velocity * time_elapsed;
	particles[index].size -= time_elapsed * 10;
	particles[index].life -= time_elapsed;
}
