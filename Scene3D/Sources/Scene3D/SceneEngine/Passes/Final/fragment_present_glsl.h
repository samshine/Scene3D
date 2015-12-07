namespace { const char *fragment_present_glsl() { return R"shaderend(

in vec2 TexCoord;
out vec4 FragColor;

uniform sampler2D FinalColors;

vec4 sRGB(vec4 c);

void main()
{
	vec4 color = texture(FinalColors, TexCoord);
	FragColor = sRGB(vec4(color.rgb, 1));
}

vec4 sRGB(vec4 c)
{
	float gamma = 1.0 / 2.2;
	return vec4(pow(c.rgb, vec3(gamma)), c.a);
	// return vec4(sqrt(c.rgb), c.a); // cheaper, but assuming gamma of 2.0 instead of 2.2
}

)shaderend"; } }
