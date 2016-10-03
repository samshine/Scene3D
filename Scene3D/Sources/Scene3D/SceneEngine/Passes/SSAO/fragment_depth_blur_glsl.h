namespace { const char *fragment_depth_blur_glsl() { return R"shaderend(

#version 330

layout(std140) uniform Uniforms
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

in vec2 TexCoord;
out vec4 FragColor;

uniform sampler2D AODepthTexture;

#define KERNEL_RADIUS 7.0

float CrossBilateralWeight(float r, float sampleDepth, float centerDepth)
{
	const float blurSigma = KERNEL_RADIUS * 0.5;
	const float blurFalloff = 1.0 / (2.0 * blurSigma * blurSigma);

	float deltaZ = (sampleDepth - centerDepth) * BlurSharpness;

	return exp2(-r * r * blurFalloff - deltaZ * deltaZ);
}

void ProcessSample(float ao, float z, float r, float centerDepth, inout float totalAO, inout float totalW)
{
	float w = CrossBilateralWeight(r, z, centerDepth);
	totalAO += w * ao;
	totalW += w;
}

void ProcessRadius(vec2 deltaUV, float centerDepth, inout float totalAO, inout float totalW)
{
	for (float r = 1; r <= KERNEL_RADIUS; r += 1.0)
	{
		vec2 uv = r * deltaUV + TexCoord;
		vec2 aoZ = texture(AODepthTexture, uv).xy;
		ProcessSample(aoZ.x, aoZ.y, r, centerDepth, totalAO, totalW);
	}
}

vec2 ComputeBlur(vec2 deltaUV)
{
	vec2 aoZ = texture(AODepthTexture, TexCoord).xy;

	float totalAO = aoZ.x;
	float totalW = 1.0;

	ProcessRadius(deltaUV, aoZ.y, totalAO, totalW);
	ProcessRadius(-deltaUV, aoZ.y, totalAO, totalW);

	return vec2(totalAO / totalW, aoZ.y);
}

vec2 BlurX()
{
	return ComputeBlur(vec2(InvFullResolution.x, 0.0));
}

float BlurY()
{
	return pow(clamp(ComputeBlur(vec2(0.0, InvFullResolution.y)).x, 0.0, 1.0), PowExponent);
}

void main()
{
#if defined(BLUR_HORIZONTAL)
	FragColor = vec4(BlurX(), 0.0, 1.0);
#else
	FragColor = vec4(BlurY(), 0.0, 0.0, 1.0);
#endif
}

)shaderend"; } }
