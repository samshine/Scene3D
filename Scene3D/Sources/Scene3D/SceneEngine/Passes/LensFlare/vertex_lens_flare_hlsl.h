namespace { const char *vertex_lens_flare_hlsl() { return R"shaderend(

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
	float2 Texcoord : PixelTexcoord;
	float DepthInEye : PixelDepth;
	float3 Color : PixelColor;
};

Texture2D InstanceTexture;

VertexOut main(VertexIn input)
{
	float4 InstanceData0 = InstanceTexture.Load(uint3(input.InstanceId, 0, 0));
	float4 positionInObject = float4(InstanceData0.xyz, 1);
	float sizeInEye = InstanceData0.w;

	float4 positionInEye = mul(ObjectToEye, positionInObject);
	positionInEye += float4(input.Position.xyz, 0) * sizeInEye;

	VertexOut output;
	output.Position = mul(EyeToProjection, positionInEye);
	output.Texcoord = input.Position.xy * 0.5 + 0.5;
	output.DepthInEye = positionInEye.z;
	output.Color = float3(1, 1, 1);
	return output;
}

)shaderend"; } }
