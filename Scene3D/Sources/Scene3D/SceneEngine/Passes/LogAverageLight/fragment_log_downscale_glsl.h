namespace { const char *fragment_log_downscale_glsl() { return R"shaderend(

#version 330

out vec4 FragAverage;
uniform sampler2D Texture;

void main()
{
	ivec2 pos = ivec2(gl_FragCoord.xy) * 2;
	float light0 = texelFetch(Texture, pos, 0).x;
	float light1 = texelFetch(Texture, pos + ivec2(1,0), 0).x;
	float light2 = texelFetch(Texture, pos + ivec2(0,1), 0).x;
	float light3 = texelFetch(Texture, pos + ivec2(1,1), 0).x;
	FragAverage = vec4((light0 + light1 + light2 + light3) * 0.25, 0, 0, 0);
}

)shaderend"; } }
