namespace { const char *fragment_log_average_glsl() { return R"shaderend(

#version 330

out vec4 FragAverage;
uniform sampler2D Texture;

void main()
{
	vec3 rgb = texture(Texture, gl_FragCoord.xy / %1).rgb;
	float light = max(max(rgb.r, rgb.g), rgb.b);
	FragAverage = vec4(log(0.0001 + light), 0, 0, 0);
}

)shaderend"; } }
