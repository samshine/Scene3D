namespace { const char *fragment_exposure_average_hlsl() { return R"shaderend(

struct PixelIn
{
	float4 ScreenPos : SV_Position;
	float2 TexCoord : PixelTexCoord;
};

struct PixelOut
{
	float4 FragColor : SV_Target0;
};

Texture2D ExposureTexture;

void main()
{
#if __VERSION__ < 400
	int2 size = textureSize(ExposureTexture, 0);
	int2 tl = max(int2(TexCoord * float2(size) - 0.5), int2(0));
	int2 br = min(tl + int2(1), size - int2(1));
	float4 values = float4(
		texelFetch(ExposureTexture, tl, 0).x,
		texelFetch(ExposureTexture, int2(tl.x, br.y), 0).x,
		texelFetch(ExposureTexture, int2(br.x, tl.y), 0).x,
		texelFetch(ExposureTexture, br, 0).x);
#else
	float4 values = textureGather(ExposureTexture, TexCoord);
#endif

	FragColor = float4((values.x + values.y + values.z + values.w) * 0.25, 0.0, 0.0, 1.0);
}

)shaderend"; } }
