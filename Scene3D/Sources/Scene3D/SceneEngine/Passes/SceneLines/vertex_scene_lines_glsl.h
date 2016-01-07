namespace { const char *vertex_scene_lines_glsl() { return R"shaderend(
#version 330

layout(std140) uniform Uniforms
{
	mat4 WorldToProjection;
};

in vec4 AttrPosition;
in vec4 AttrColor;

out vec4 PixelColor;

void main()
{
	gl_Position = WorldToProjection * AttrPosition;
	PixelColor = AttrColor;
}

)shaderend"; } }
