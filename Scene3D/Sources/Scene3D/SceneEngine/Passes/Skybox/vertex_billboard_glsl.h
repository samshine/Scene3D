namespace { const char *vertex_billboard_glsl() { return R"shaderend(
#version 330

in vec4 AttrPosition;
out vec2 PixelUV;
out float PixelDistance;

layout(std140) uniform Uniforms
{
	mat4 ObjectToEye;
	mat4 EyeToProjection;
};

uniform sampler2D InstanceTexture;

void main()
{
	vec4 InstanceData = texelFetch(InstanceTexture, ivec2(gl_InstanceID, 0), 0);
	vec4 PositionInEye = ObjectToEye * vec4(InstanceData.xyz, 1);
	float Distance = length(PositionInEye);
	PositionInEye += vec4(AttrPosition.xyz, 0) * InstanceData.w;

	gl_Position = EyeToProjection * PositionInEye;
	PixelUV = AttrPosition.xy * 0.5 + 0.5;
	PixelDistance = (1 - Distance / 1000) * 0.7;
}

)shaderend"; } }
