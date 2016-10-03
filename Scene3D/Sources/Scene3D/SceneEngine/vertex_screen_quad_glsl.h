namespace { const char *vertex_screen_quad_glsl() { return R"shaderend(

in vec4 PositionInProjection;
out vec2 TexCoord;

void main()
{
	gl_Position = PositionInProjection;
	TexCoord = PositionInProjection.xy * 0.5 + 0.5;
}

)shaderend"; } }
