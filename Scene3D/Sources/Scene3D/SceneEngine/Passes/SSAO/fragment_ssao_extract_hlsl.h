namespace {
	const char *fragment_ssao_extract_hlsl() {
		return R"shaderend(

cbuffer Uniforms
{
	float f;
	float f_div_aspect;
	float padding0, padding1;
	float4 sampledata[160];
};

struct PixelIn
{
	float4 ScreenPos : SV_Position;
	float2 TexCoord : TexCoord;
};

struct PixelOut
{
	float4 FragColor : SV_Target0;
};

Texture2D NormalZ;
SamplerState NormalZSampler;

float SampleAttenuation(uint sampleIndex, float3 positionInEye);

PixelOut main(PixelIn input)
{
	PixelOut output;
	uint2 screenPos = uint2(input.ScreenPos.xy);
	uint patchStart = (screenPos.x % 4 + (screenPos.y % 4) * 4) * 10;

	float2 positionProjected = input.TexCoord * 2 - 1;
	float3 positionInEye = float3(positionProjected.x / f_div_aspect, positionProjected.y / f, 1) * NormalZ.Sample(NormalZSampler, input.TexCoord).w;

	float attenuation =
		SampleAttenuation(patchStart + 0, positionInEye) +
		SampleAttenuation(patchStart + 1, positionInEye) +
		SampleAttenuation(patchStart + 2, positionInEye) +
		SampleAttenuation(patchStart + 3, positionInEye) +
		SampleAttenuation(patchStart + 4, positionInEye) +
		SampleAttenuation(patchStart + 5, positionInEye) +
		SampleAttenuation(patchStart + 6, positionInEye) +
		SampleAttenuation(patchStart + 7, positionInEye) +
		SampleAttenuation(patchStart + 8, positionInEye) +
		SampleAttenuation(patchStart + 9, positionInEye);

	output.FragColor.x = attenuation;

	return output;
}

float SampleAttenuation(uint sampleIndex, float3 positionInEye)
{
	float4 sample = sampledata[sampleIndex];
	float3 samplePositionInEye = positionInEye + sample.xyz;
	float2 sampleNormalized = float2(samplePositionInEye.x * f_div_aspect, samplePositionInEye.y * f) / samplePositionInEye.z;
	float depthAtSamplePosition = NormalZ.Sample(NormalZSampler, sampleNormalized * 0.5 + 0.5).w;
	float occluded = step(depthAtSamplePosition, samplePositionInEye.z);
	float notFarAway = step(samplePositionInEye.z, depthAtSamplePosition + 0.7);
	return (1 - occluded * notFarAway) * sample.w;
}

)shaderend"; } }
