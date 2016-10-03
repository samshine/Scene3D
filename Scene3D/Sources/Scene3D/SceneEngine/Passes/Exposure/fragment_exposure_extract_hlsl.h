namespace { const char *fragment_exposure_extract_hlsl() { return R"shaderend(

cbuffer Uniforms
{
	float ExposureBase;
	float ExposureMin;
	float ExposureScale;
	float ExposureSpeed;
	float2 Scale;
	float2 Offset;
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

Texture2D SceneTexture;
SamplerState SceneTextureSampler;

void main()
{
	float4 color = SceneTexture.Sample(SceneTextureSampler, Offset + TexCoord * Scale);
	FragColor = float4(max(max(color.r, color.g), color.b), 0.0, 0.0, 1.0);
	//FragColor = float4(log(0.0001 + max(max(color.r, color.g), color.b)), 0.0, 0.0, 1.0);
}

)shaderend"; } }
