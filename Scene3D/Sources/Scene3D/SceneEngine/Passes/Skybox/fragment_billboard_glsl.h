namespace { const char *fragment_billboard_glsl() { return R"shaderend(
#version 330

in vec2 PixelUV;
in float PixelDistance;

out vec4 FragColor;

uniform sampler2D ParticleTexture;

void main()
{
	FragColor = texture(ParticleTexture, PixelUV);
	FragColor.xyz *= PixelDistance;
}

)shaderend"; } }
