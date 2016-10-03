namespace { const char *fragment_lens_distortion_hlsl() { return R"shaderend(

cbuffer Uniforms
{
	float Aspect; // image width/height
	float Scale;  // 1/max(f)
	float Padding1, Padding2;
	float4 k;       // lens distortion coefficient 
	float4 kcube;   // cubic distortion value
};

struct PixelIn
{
	float4 ScreenPos : SV_Position;
	float2 TexCoord : PixelTexCoord;
};

struct PixelOut
{
	float4 FragColor : SV_Target0;
};

Texture2D InputTexture;
SamplerState InputTextureSampler;

PixelOut main(PixelIn input)
{
	PixelOut output;

	float2 position = input.TexCoord - 0.5;

	float2 p = float2(position.x * Aspect, position.y);
	float r2 = dot(p, p);
	float3 f = 1.0 + r2 * (k.rgb + kcube.rgb * sqrt(r2));

	float3 x = f * position.x * Scale + 0.5;
	float3 y = f * position.y * Scale + 0.5;

	float3 c;
	c.r = InputTexture.Sample(InputTextureSampler, float2(x.r, y.r)).r;
	c.g = InputTexture.Sample(InputTextureSampler, float2(x.g, y.g)).g;
	c.b = InputTexture.Sample(InputTextureSampler, float2(x.b, y.b)).b;

	output.FragColor = float4(c, 1.0);
	return output;
}

)shaderend"; } }
