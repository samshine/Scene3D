namespace { const char *fragment_scene_lines_glsl() { return R"shaderend(
#version 330

layout(std140) uniform Uniforms
{
	mat4 WorldToProjection;
};

in vec4 PixelColor;
out vec4 FragColor;

void main()
{
	FragColor = PixelColor;
}

)shaderend"; } }
