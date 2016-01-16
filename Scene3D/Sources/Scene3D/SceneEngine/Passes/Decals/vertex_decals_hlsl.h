namespace { const char *vertex_decals_hlsl() { return R"shaderend(

cbuffer Uniforms
{
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
	float2 Texcoord : PixelTexcoord;
	float Cutoff : PixelCutoff;
	float Glossiness : PixelGlossiness;
	float SpecularLevel : PixelSpecularLevel;
};

Texture2D InstanceTexture;

float4 InstanceData(VertexIn input, int offset);
float4x4 InstanceMat4(VertexIn input, int offset);

VertexOut main(VertexIn input)
{
	float4x4 objectToEye = InstanceMat4(input, 0);
	float4 instanceData3 = InstanceData(input, 3);

	VertexOut output;
	output.Position = mul(EyeToProjection, mul(objectToEye, input.Position));
	output.Texcoord = input.Position.xy * 0.5 + 0.5;
	output.Cutoff = instanceData3.x;
	output.Glossiness = instanceData3.y;
	output.SpecularLevel = instanceData3.z;
	return output;
}

float4x4 InstanceMat4(VertexIn input, int offset)
{
	return float4x4(InstanceData(input, 0), InstanceData(input, 1), InstanceData(input, 2), float4(0, 0, 0, 1));
}

float4 InstanceData(VertexIn input, int offset)
{
	offset += input.InstanceId * 4;
	return InstanceTexture.Load(uint3(offset % 256, offset / 256, 0));
}

)shaderend"; } }
