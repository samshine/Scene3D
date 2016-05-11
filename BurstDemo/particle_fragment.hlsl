
struct PixelIn
{
	float4 ScreenPos : SV_Position;
	uint InstanceId : PixelInstanceId;
	float2 UV : PixelUV;
};

struct PixelOut
{
	float4 FragColor : SV_Target0;
};

struct Particle // Needs to match particle.hlsl
{
	float3 pos;
	float size;
	float3 velocity;
	float life;
};

Texture2D ParticleTexture;
SamplerState ParticleSampler;

StructuredBuffer<Particle> particles;

PixelOut main(PixelIn input)
{
	Particle particle = particles[input.InstanceId];

	PixelOut output;

	float4 color = ParticleTexture.Sample(ParticleSampler, input.UV);

	output.FragColor = color;
	return output;
}
