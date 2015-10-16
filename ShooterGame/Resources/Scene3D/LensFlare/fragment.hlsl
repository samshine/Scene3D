
cbuffer Uniforms
{
	float4x4 ObjectToEye;
	float4x4 EyeToProjection;
};

struct PixelIn
{
	float4 ScreenPos : SV_Position;
	float2 Texcoord : PixelTexcoord;
	float DepthInEye : PixelDepth;
	float3 Color : PixelColor;
};

struct PixelOut
{
	float4 FragColor : SV_Target0;
};

Texture2D FlareTexture;
SamplerState FlareSampler;

PixelOut main(PixelIn input)
{
	float4 textureColor = FlareTexture.Sample(FlareSampler, input.Texcoord);

	PixelOut output;
	output.FragColor = textureColor * float4(input.Color, 1);
	return output;
}
