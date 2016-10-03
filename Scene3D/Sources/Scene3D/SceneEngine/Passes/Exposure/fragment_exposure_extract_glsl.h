namespace { const char *fragment_exposure_extract_glsl() { return R"shaderend(

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

uniform sampler2D SceneTexture;

void main()
{
	vec4 color = texture(SceneTexture, Offset + TexCoord * Scale);
	FragColor = vec4(max(max(color.r, color.g), color.b), 0.0, 0.0, 1.0);
	//FragColor = vec4(log(0.0001 + max(max(color.r, color.g), color.b)), 0.0, 0.0, 1.0);
}

)shaderend"; } }
