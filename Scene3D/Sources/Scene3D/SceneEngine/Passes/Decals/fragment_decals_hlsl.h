namespace { const char *fragment_decals_hlsl() { return R"shaderend(

cbuffer Uniforms
{
	float4x4 EyeToProjection;
};

struct PixelIn
{
	float4 ScreenPos : SV_Position;
	float2 Texcoord : PixelTexcoord;
	float Cutoff : PixelCutoff;
	float Glossiness : PixelGlossiness;
	float SpecularLevel : PixelSpecularLevel;
};

struct PixelOut
{
	float4 FragColor : SV_Target0;
};

Texture2D DiffuseTexture;
SamplerState DiffuseSampler;

PixelOut main(PixelIn input)
{
	float4 textureColor = DiffuseTexture.Sample(DiffuseSampler, input.Texcoord);

	PixelOut output;
	output.FragColor = textureColor;
	return output;
}

)shaderend"; } }
