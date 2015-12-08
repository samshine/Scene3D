namespace { const char *fragment_exposure_glsl() { return R"shaderend(

#version 330

out vec4 FragAverage;
uniform sampler2D TexturePrev;
uniform sampler2D TextureCurrent;

void main()
{
	float lightPrev = texelFetch(TexturePrev, ivec2(0,0), 0).x;
	float lightCurrent = exp(texelFetch(TextureCurrent, ivec2(0,0), 0).x);
	float c = 0.04;
	FragAverage = vec4(lightPrev + c * (lightCurrent - lightPrev), 0, 0, 0);
}

)shaderend"; } }
