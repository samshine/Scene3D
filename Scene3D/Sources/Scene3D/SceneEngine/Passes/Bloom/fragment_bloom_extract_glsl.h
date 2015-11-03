namespace { const char *fragment_bloom_extract_glsl() { return R"shaderend(

in vec2 TexCoord;
out vec4 FragColor;

uniform sampler2D FinalColors;

void main()
{
	vec4 color = texture(FinalColors, TexCoord);
	const float blurBegin = 1.0;
	const float blurEnd = 2.0;
	FragColor = vec4(clamp((color.rgb - blurBegin) / (blurEnd - blurBegin), 0, 1), 0);
}

)shaderend"; } }
