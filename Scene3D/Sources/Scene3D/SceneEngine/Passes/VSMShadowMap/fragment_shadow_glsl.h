namespace { const char *fragment_shadow_glsl() { return R"shaderend(

out vec2 FragMoment;

void main()
{
	FragMoment = vec2(0.0);
}

)shaderend"; } }
