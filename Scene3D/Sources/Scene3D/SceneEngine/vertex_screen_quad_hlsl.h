namespace { const char *vertex_screen_quad_hlsl() { return R"shaderend(

struct VertexIn
{
	float4 PositionInProjection : PositionInProjection;
};

struct VertexOut
{
	float4 PositionInProjection : SV_Position;
	float2 TexCoord : PixelTexCoord;
};

VertexOut main(VertexIn input)
{
	VertexOut output;
	output.PositionInProjection = input.PositionInProjection;
	output.TexCoord = input.PositionInProjection * 0.5 + 0.5;
	output.TexCoord.y = 1.0 - output.TexCoord.y;
	return output;
}

)shaderend"; } }
