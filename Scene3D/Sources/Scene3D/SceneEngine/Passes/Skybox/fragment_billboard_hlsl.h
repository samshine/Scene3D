namespace { const char *fragment_billboard_hlsl() { return R"shaderend(

struct PixelIn
{
	float4 Position : SV_Position;
	float2 UV : PixelUV;
	float Distance : PixelDistance;
};

struct PixelOut
{
	float4 FragColor : SV_Target0;
};

Texture2D ParticleTexture;
SamplerState ParticleTextureSampler;

PixelOut main(PixelIn input)
{
	PixelOut output;
	output.FragColor = ParticleTexture.Sample(ParticleTextureSampler, input.UV);
	output.FragColor.xyz *= input.Distance;
	return output;
}

)shaderend"; } }
