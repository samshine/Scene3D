namespace { const char *fragment_bloom_combine_hlsl() { return R"shaderend(

struct PixelIn
{
	float4 ScreenPos : SV_Position;
	float2 TexCoord : TexCoord;
};

struct PixelOut
{
	float4 FragColor : SV_Target0;
};

Texture2D Bloom;
SamplerState BloomSampler;

PixelOut main(PixelIn input)
{
	PixelOut output;
	output.FragColor = Bloom.Sample(BloomSampler, input.TexCoord);
	return output;
}

)shaderend"; } }
