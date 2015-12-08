namespace { const char *vertex_log_average_glsl() { return R"shaderend(

#version 330
in vec4 PositionInProjection;
void main()
{
	gl_Position = PositionInProjection;
}

)shaderend"; } }
