namespace { const char *fragment_bloom_extract_hlsl() { return R"shaderend(

struct PixelIn
{
	float4 ScreenPos : SV_Position;
	float2 TexCoord : TexCoord;
};

struct PixelOut
{
	float4 FragColor : SV_Target0;
};

Texture2D FinalColors;
SamplerState FinalColorsSampler;

PixelOut main(PixelIn input)
{
	PixelOut output;
	float4 color = FinalColors.Sample(FinalColorsSampler, input.TexCoord);
	float exposureAdjustment = 2;
	output.FragColor = float4(color.rgb * exposureAdjustment - 1, 1);
	return output;
}

)shaderend"; } }
