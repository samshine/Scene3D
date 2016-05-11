
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

	if (particles[index].life <= 0)
	{
		particles[index].pos = float3(0, 0, 10) + float3(-cos(radians(index)), -sin(radians(index)), -cos(radians(index))) * 4;
		particles[index].velocity.x = cos(radians(index)) * 2 * 0.016666f;
		particles[index].velocity.y = sin(radians(index)) * 2 * 0.016666f;
		particles[index].life = 1;
	}
	else
	{
		particles[index].pos += particles[index].velocity;
		particles[index].size = 0.01;// (1 + cos(radians(particles[index].velocity.x))) * 0.05;
	}
}
