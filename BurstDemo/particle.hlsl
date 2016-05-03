
struct Particle
{
	float3 pos;
	float padding;
	float3 velocity;
	float life;
};

RWStructuredBuffer<Particle> particles;

[numthreads(1, 1, 1)]
void main(uint3 threadId : SV_DispatchThreadID )
{
	uint index = threadId.x;
	particles[index].pos += particles[index].velocity;
	particles[index].life = max(particles[index].life - 0.016666f, 0.0f);
}
