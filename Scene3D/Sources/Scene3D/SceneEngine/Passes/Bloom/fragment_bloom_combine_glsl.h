namespace { const char *fragment_bloom_combine_glsl() { return R"shaderend(

in vec2 TexCoord;
out vec4 FragColor;

uniform sampler2D Bloom;

void main()
{
	FragColor = texture(Bloom, TexCoord);
}

)shaderend"; } }
