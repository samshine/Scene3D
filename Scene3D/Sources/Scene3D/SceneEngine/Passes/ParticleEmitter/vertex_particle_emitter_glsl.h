namespace { const char *vertex_particle_emitter_glsl() { return R"shaderend(
#version 330

layout(std140) uniform Uniforms
{
	mat4 ObjectToEye;
	mat4 EyeToProjection;
	float RcpDepthFadeDistance;
	int InstanceVectorsOffset;
};

in vec4 AttrPosition;

out vec3 PixelTexcoord;
out float PixelDepth;
out vec3 PixelColor;

uniform sampler2D InstanceTexture;
uniform sampler2D ColorGradientTexture;

void main()
{
	vec4 InstanceData0 = texelFetch(InstanceTexture, ivec2(InstanceVectorsOffset + gl_InstanceID * 2, 0), 0);
	vec4 InstanceData1 = texelFetch(InstanceTexture, ivec2(InstanceVectorsOffset + gl_InstanceID * 2 + 1, 0), 0);
	vec4 positionInObject = vec4(InstanceData0.xyz, 1);
	float sizeInEye = InstanceData0.w;
	float life = InstanceData1.x;

	if (life >= 0)
	{
		vec4 positionInEye = ObjectToEye * positionInObject;
		positionInEye += vec4(AttrPosition.xyz, 0) * sizeInEye;

		gl_Position = EyeToProjection * positionInEye;
		PixelTexcoord = vec3(AttrPosition.xy * 0.5 + 0.5, life);
		PixelDepth = positionInEye.z;
		PixelColor = texture(ColorGradientTexture, vec2(life, 0), 0).rgb;
	}
	else
	{
		gl_Position = vec4(0,0,0,0);
	}
}

)shaderend"; } }
