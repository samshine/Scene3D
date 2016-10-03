namespace { const char *fragment_linear_depth_hlsl() { return R"shaderend(

cbuffer Uniforms
{
	float LinearizeDepthA;
	float LinearizeDepthB;
	float InverseDepthRangeA;
	float InverseDepthRangeB;
	float2 Scale;
	float2 Offset;
	int SampleCount;
};

struct PixelIn
{
	float4 ScreenPos : SV_Position;
	float2 TexCoord : PixelTexCoord;
};

struct PixelOut
{
	float4 FragColor : SV_Target0;
};

#if defined(MULTISAMPLE)
Texture2DMS DepthTexture;
Texture2DMS ColorTexture;
#else
Texture2D DepthTexture;
Texture2D ColorTexture;
#endif

PixelOut main(PixelIn input)
{
	PixelOut output;

	float2 uv = Offset + input.TexCoord * Scale;

#if defined(MULTISAMPLE)
	float texWidth, texHeight;
	DepthTexture.GetDimensions(texWidth, texHeight);
	float2 texSize = float2(texWidth, texHeight);
#else
	float texWidth, texHeight, texSamples;
	DepthTexture.GetDimensions(texWidth, texHeight, texSamples);
	float2 texSize = float2(texWidth, texHeight);
#endif

	// Use floor here because as we downscale the sampling error has to remain uniform to prevent
	// noise in the depth values.
	int2 ipos = int2(max(floor(uv * texSize - 0.75), float2(0.0)));

#if defined(MULTISAMPLE)
	float depth = 0.0;
	for (int i = 0; i < SampleCount; i++)
		depth += ColorTexture.Load(uint3(ipos, i)).a != 0.0 ? DepthTexture.Load(uint3(ipos, i)).x : 1.0;
	depth /= float(SampleCount);
#else
	float depth = ColorTexture.Load(uint3(ipos, 0)).a != 0.0 ? DepthTexture.Load(uint3(ipos, 0)).x : 1.0;
#endif

	float normalizedDepth = saturate(InverseDepthRangeA * depth + InverseDepthRangeB);
	output.FragColor = float4(1.0 / (normalizedDepth * LinearizeDepthA + LinearizeDepthB), 0.0, 0.0, 1.0);
	return output;
}

)shaderend"; } }
