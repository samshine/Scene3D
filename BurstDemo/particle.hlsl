
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

	float time_elapsed = 0.016666f;

	if (particles[index].life <= 0)
	{
		particles[index].pos = float3(0, 0, 10) + float3(-sin(radians(index)), -cos(radians(index)), -sin(radians(index))) * 4;
		particles[index].velocity.x = cos(radians(index)) * 2;
		particles[index].velocity.y = sin(radians(index)) * 2;
		particles[index].life = 10;
		particles[index].size = 1;
	}
	else
	{
		particles[index].pos += particles[index].velocity * time_elapsed;
		particles[index].life -= time_elapsed;
	}
}
