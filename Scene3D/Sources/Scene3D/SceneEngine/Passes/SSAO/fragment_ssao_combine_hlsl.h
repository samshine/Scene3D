namespace { const char *fragment_ssao_combine_hlsl() { return R"shaderend(

cbuffer Uniforms
{
	vec2 UVToViewA;
	vec2 UVToViewB;
	vec2 InvFullResolution;
	float NDotVBias;
	float NegInvR2;
	float RadiusToScreen;
	float AOMultiplier;
	float AOStrength;
	float BlurSharpness;
	float PowExponent;
	float Padding1, Padding2, Padding3;
	vec2 Scale;
	vec2 Offset;
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

uniform sampler2D AODepthTexture;

#if defined(MULTISAMPLE)
uniform sampler2DMS SceneDataTexture;
uniform int SampleCount;
#else
uniform sampler2D SceneDataTexture;
#endif

PixelOut main(PixelIn input)
{
	PixelOut output;

	vec2 uv = Offset + TexCoord * Scale;
#if defined(MULTISAMPLE)
	ivec2 texSize = textureSize(SceneDataTexture);
#else
	ivec2 texSize = textureSize(SceneDataTexture, 0);
#endif
	ivec2 ipos = ivec2(max(floor(uv * vec2(texSize) - 0.75), vec2(0.0)));

#if defined(MULTISAMPLE)
	vec3 fogColor = vec3(0.0);
	for (int i = 0; i < SampleCount; i++)
		fogColor += texelFetch(SceneDataTexture, ipos, i).rgb;
	fogColor /= float(SampleCount);
#else
	vec3 fogColor = texelFetch(SceneDataTexture, ipos, 0).rgb;
#endif

	float attenutation = texture(AODepthTexture, TexCoord).x;
	output.FragColor = vec4(fogColor, 1.0 - attenutation);

	return output;
}

)shaderend"; } }
