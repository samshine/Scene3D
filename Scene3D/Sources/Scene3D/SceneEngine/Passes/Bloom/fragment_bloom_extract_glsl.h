namespace { const char *fragment_bloom_extract_glsl() { return R"shaderend(

in vec2 TexCoord;
out vec4 FragColor;

uniform sampler2D FinalColors;

void main()
{
	vec4 color = texture(FinalColors, TexCoord);
	float exposureAdjustment = 1.5;
	FragColor = vec4(color.rgb * exposureAdjustment - 1, 1);
}

)shaderend"; } }
