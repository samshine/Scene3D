
struct Particle
{
	float3 pos;
	float size;
	float3 velocity;
	float life;
};

RWStructuredBuffer<Particle> particles;

[numthreads(1, 1, 1)]
void main(uint3 threadId : SV_DispatchThreadID )
{
	uint index = threadId.x;

	particles[index].pos.x += cos(radians(particles[index].velocity.x + index)) * 2 * 0.016666f;
	particles[index].pos.y += sin(radians(particles[index].velocity.x + index)) * 2 * 0.016666f;
	particles[index].pos.z = 10;
	particles[index].velocity.x += 1;
	particles[index].size = 1;

	/*
	particles[index].pos += particles[index].velocity;
	particles[index].life = max(particles[index].life - 0.016666f, 0.0f);
	*/
}
