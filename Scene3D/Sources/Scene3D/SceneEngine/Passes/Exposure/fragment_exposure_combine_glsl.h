namespace { const char *fragment_exposure_combine_glsl() { return R"shaderend(

#version 330

layout(std140) uniform Uniforms
{
	float ExposureBase;
	float ExposureMin;
	float ExposureScale;
	float ExposureSpeed;
	vec2 Scale;
	vec2 Offset;
};

in vec2 TexCoord;
out vec4 FragColor;

uniform sampler2D ExposureTexture;

void main()
{
	float light = texture(ExposureTexture, TexCoord).x;
	float exposureAdjustment = 1.0 / max(ExposureBase + light * ExposureScale, ExposureMin);
	FragColor = vec4(exposureAdjustment, 0.0, 0.0, ExposureSpeed);
}

)shaderend"; } }
