namespace { const char *fragment_sphere_glsl() { return R"shaderend(
#version 330

in vec3 CubeDirection;
out vec4 FragColor;

uniform sampler2D SkyboxTexture;

void main()
{
	vec2 uv = normalize(CubeDirection).xy * 0.5 + 0.5;

	FragColor = texture(SkyboxTexture, uv);
}

)shaderend"; } }
