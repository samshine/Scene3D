namespace { const char *fragment_exposure_combine_hlsl() { return R"shaderend(

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

Texture2D ExposureTexture;
SamplerState ExposureTextureSampler;

void main()
{
	float light = ExposureTexture.Sample(ExposureTextureSampler, TexCoord).x;
	float exposureAdjustment = 1.0 / max(ExposureBase + light * ExposureScale, ExposureMin);
	FragColor = float4(exposureAdjustment, 0.0, 0.0, ExposureSpeed);
}

)shaderend"; } }
