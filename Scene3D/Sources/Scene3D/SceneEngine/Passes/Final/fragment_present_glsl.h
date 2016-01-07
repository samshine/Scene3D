namespace { const char *fragment_present_glsl() { return R"shaderend(

in vec2 TexCoord;
out vec4 FragColor;

uniform sampler2D FinalColors;

vec3 sRGB(vec3 c);
vec3 tonemapLinear(vec3 color);
vec3 tonemapReinhard(vec3 color);
vec3 tonemapHejlDawson(vec3 color);
vec3 tonemapUncharted2(vec3 color);

void main()
{
	vec3 color = texture(FinalColors, TexCoord).rgb;
	float exposureAdjustment = 1.5;
	color = color * exposureAdjustment;
	FragColor = vec4(tonemapUncharted2(color), 1);
}

vec3 tonemapLinear(vec3 color)
{
	return sRGB(color);
}

vec3 tonemapReinhard(vec3 color)
{
	color = color / (1 + color);
	return sRGB(color);
}

vec3 tonemapHejlDawson(vec3 color)
{
	vec3 x = max(vec3(0), color - 0.004);
	return (x * (6.2 * x + 0.5)) / (x * (6.2 * x + 1.7) + 0.06); // no sRGB needed
}

vec3 Uncharted2Tonemap(vec3 x)
{
	float A = 0.15;
	float B = 0.50;
	float C = 0.10;
	float D = 0.20;
	float E = 0.02;
	float F = 0.30;
	return ((x * (A * x + C * B) + D * E) / (x * (A * x + B) + D * F)) - E / F;
}

vec3 tonemapUncharted2(vec3 color)
{
	float W = 11.2;
	float ExposureBias = 2.0;
	vec3 curr = Uncharted2Tonemap(ExposureBias * color);
	vec3 whiteScale = vec3(1) / Uncharted2Tonemap(vec3(W));
	return sRGB(curr * whiteScale);
}

vec3 sRGB(vec3 c)
{
	return pow(c.rgb, vec3(1.0 / 2.2));
	// return sqrt(c.rgb); // cheaper, but assuming gamma of 2.0 instead of 2.2
}

)shaderend"; } }
