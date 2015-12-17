namespace { const char *vertex_scene_lines_hlsl() { return R"shaderend(

cbuffer Uniforms
{
	float4x4 WorldToProjection;
}

struct VertexIn
{
	float4 Position : AttrPosition;
	float4 Color : AttrColor;
};

struct VertexOut
{
	float4 Position : SV_Position;
	float4 Color : PixelColor;
};

VertexOut main(VertexIn input)
{
	VertexOut output;
	output.Position = mul(WorldToProjection, input.Position);
	output.Color = input.Color;
	return output;
}

)shaderend"; } }
