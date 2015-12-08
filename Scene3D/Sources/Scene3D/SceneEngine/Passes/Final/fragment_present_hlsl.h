namespace {
	const char *fragment_present_hlsl() {
		return R"shaderend(

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

float3 sRGB(float3 c);
float3 tonemapLinear(float3 color);
float3 tonemapReinhard(float3 color);
float3 tonemapHejlDawson(float3 color);
float3 tonemapUncharted2(float3 color);

PixelOut main(PixelIn input)
{
	PixelOut output;
	float3 color = FinalColors.Sample(FinalColorsSampler, input.TexCoord).rgb;
	float exposureAdjustment = 2;
	color = color * exposureAdjustment;
	output.FragColor = float4(tonemapUncharted2(color), 1);
	return output;
}

float3 tonemapLinear(float3 color)
{
	return sRGB(color);
}

float3 tonemapReinhard(float3 color)
{
	color = color / (1 + color);
	return sRGB(color);
}

float3 tonemapHejlDawson(float3 color)
{
	float3 x = max(0, color - 0.004);
	return (x * (6.2 * x + 0.5)) / (x * (6.2 * x + 1.7) + 0.06); // no sRGB needed
}

float3 Uncharted2Tonemap(float3 x)
{
	float A = 0.15;
	float B = 0.50;
	float C = 0.10;
	float D = 0.20;
	float E = 0.02;
	float F = 0.30;
	return ((x * (A * x + C * B) + D * E) / (x * (A * x + B) + D * F)) - E / F;
}

float3 tonemapUncharted2(float3 color)
{
	float W = 11.2;
	float ExposureBias = 2.0f;
	float3 curr = Uncharted2Tonemap(ExposureBias * color);
	float3 whiteScale = 1.0f / Uncharted2Tonemap(W);
	return sRGB(curr * whiteScale);
}

float3 sRGB(float3 c)
{
	return pow(c.rgb, 1.0 / 2.2);
	// return sqrt(c.rgb); // cheaper, but assuming gamma of 2.0 instead of 2.2
}

)shaderend"; } }
