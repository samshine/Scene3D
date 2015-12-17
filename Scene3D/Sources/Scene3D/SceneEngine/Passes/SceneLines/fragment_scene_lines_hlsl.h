namespace { const char *fragment_scene_lines_hlsl() { return R"shaderend(

cbuffer Uniforms
{
	float4x4 WorldToProjection;
};

struct PixelIn
{
	float4 ScreenPos : SV_Position;
	float4 Color : PixelColor;
};

struct PixelOut
{
	float4 FragColor : SV_Target0;
};

PixelOut main(PixelIn input)
{
	PixelOut output;
	output.FragColor = input.Color;
	return output;
}

)shaderend"; } }
