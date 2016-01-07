namespace { const char *vertex_cube_glsl() { return R"shaderend(
#version 330

in vec4 AttrPosition;
out vec3 CubeDirection;

layout(std140) uniform Uniforms
{
	mat4 ObjectToEye;
	mat4 EyeToProjection;
};

void main()
{
	gl_Position = EyeToProjection * ObjectToEye * AttrPosition;
	CubeDirection = AttrPosition.xyz;
}

)shaderend"; } }
