namespace { const char *fragment_shadow_glsl() { return R"shaderend(

in vec4 PositionInEye;
out vec4 FragMoment;

void main()
{
	float distanceToLight = length(PositionInEye);
	FragMoment = vec4(distanceToLight, distanceToLight * distanceToLight, 0.0f, 0.0f);
}

)shaderend"; } }
