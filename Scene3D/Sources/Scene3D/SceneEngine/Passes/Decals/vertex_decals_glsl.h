namespace { const char *vertex_decals_glsl() { return R"shaderend(
#version 330

layout(std140) uniform Uniforms
{
	mat4 EyeToProjection;
	float rcp_f;
	float rcp_f_div_aspect;
	vec2 two_rcp_viewport_size;
};

in vec4 AttrPosition;

out float Cutoff;
out float Glossiness;
out float SpecularLevel;
out vec3 BoxCenter;
out vec4 BoxX;
out vec4 BoxY;
out vec4 BoxZ;

uniform sampler2D InstanceTexture;

vec4 InstanceData(int offset);
mat4 InstanceMat4(int offset);

void main()
{
	mat4 objectToEye = InstanceMat4(0);
	vec4 instanceData3 = InstanceData(3);

	gl_Position = EyeToProjection * objectToEye * AttrPosition;
	Cutoff = instanceData3.x;
	Glossiness = instanceData3.y;
	SpecularLevel = instanceData3.z;
	BoxCenter = (objectToEye * vec4(0,0,0,1)).xyz;
	BoxX.xyz = (objectToEye * vec4(1,0,0,1)).xyz - BoxCenter;
	BoxY.xyz = (objectToEye * vec4(0,-1,0,1)).xyz - BoxCenter;
	BoxZ.xyz = (objectToEye * vec4(0,0,1,1)).xyz - BoxCenter;
	BoxX.w = 1 / length(BoxX.xyz);
	BoxY.w = 1 / length(BoxY.xyz);
	BoxZ.w = 1 / length(BoxZ.xyz);
	BoxX.xyz *= BoxX.w;
	BoxY.xyz *= BoxY.w;
	BoxZ.xyz *= BoxZ.w;
}

mat4 InstanceMat4(int offset)
{
	return transpose(mat4(InstanceData(0), InstanceData(1), InstanceData(2), vec4(0, 0, 0, 1)));
}

vec4 InstanceData(int offset)
{
	offset += gl_InstanceID * 4;
	return texelFetch(InstanceTexture, ivec2(offset % 256, offset / 256), 0);
}

)shaderend"; } }
