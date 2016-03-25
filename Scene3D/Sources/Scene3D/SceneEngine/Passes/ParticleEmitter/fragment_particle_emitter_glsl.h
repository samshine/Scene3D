namespace { const char *fragment_particle_emitter_glsl() { return R"shaderend(
#version 330

layout(std140) uniform Uniforms
{
	mat4 ObjectToEye;
	mat4 EyeToProjection;
	float RcpDepthFadeDistance;
	int InstanceVectorsOffset;
};

in vec3 PixelTexcoord;
in float PixelDepth;
in vec3 PixelColor;

out vec4 FragColor;

uniform sampler3D ParticleTexture;
uniform sampler2D FaceNormalZTexture;

void main()
{
	vec4 textureColor = texture(ParticleTexture, PixelTexcoord);
	float fragmentDepthInEye = texelFetch(FaceNormalZTexture, ivec2(gl_FragCoord.xy), 0).w;

	float depthDiff = fragmentDepthInEye - PixelDepth;
	float depthFade = clamp(depthDiff * RcpDepthFadeDistance, 0, 1);

	// Workaround since skybox fragments has 1 as depth value
	if (fragmentDepthInEye == 1)
		depthFade = 1;

	// To do: apply light on particles

	FragColor = textureColor * vec4(PixelColor, 1) * depthFade;
}

)shaderend"; } }
