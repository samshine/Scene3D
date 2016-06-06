
cbuffer Uniforms
{
	float4x4 ObjectToEye;
	float4x4 EyeToProjection;
}

struct VertexIn
{
	float4 Position : AttrPosition;
	uint InstanceId : SV_InstanceID;
};

struct VertexOut
{
	float4 Position : SV_Position;
	uint InstanceId : PixelInstanceId;
	float2 UV : PixelUV;
};

struct Particle // Needs to match particle.hlsl
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

StructuredBuffer<Particle> particles;

VertexOut main(VertexIn input)
{
	Particle particle = particles[input.InstanceId];

	VertexOut output;
	if (particle.life >= 0)
	{
		float4 positionInEye = mul(ObjectToEye, float4(particle.pos, 1));

		positionInEye += float4(input.Position.xyz, 0) * particle.size;

		output.Position = mul(EyeToProjection, positionInEye);
		output.UV = input.Position.xy * 0.5 + 0.5;
		output.InstanceId = input.InstanceId;
	}
	else
	{
		output.Position = float4(0, 0, 0, 0);
		output.UV = float2(0, 0);
		output.InstanceId = input.InstanceId;
	}
	return output;
}
