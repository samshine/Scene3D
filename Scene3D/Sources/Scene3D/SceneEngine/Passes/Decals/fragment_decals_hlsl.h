namespace { const char *fragment_decals_hlsl() { return R"shaderend(

// Direct3D's render targets are top-down, while OpenGL uses bottom-up
#define TOP_DOWN_RENDER_TARGET

cbuffer Uniforms
{
	float4x4 EyeToProjection;
	float rcp_f;
	float rcp_f_div_aspect;
	float2 two_rcp_viewport_size;
	int instance_base;
};

struct PixelIn
{
	float4 ScreenPos : SV_Position;
	float Cutoff : PixelCutoff;
	float Glossiness : PixelGlossiness;
	float SpecularLevel : PixelSpecularLevel;
	float3 BoxCenter : PixelBoxCenter;
	float4 BoxX : PixelBoxX;
	float4 BoxY : PixelBoxY;
	float4 BoxZ : PixelBoxZ;
};

struct PixelOut
{
	float4 FragColor : SV_Target0;
};

Texture2D FaceNormalZTexture;

Texture2D DiffuseTexture;
SamplerState DiffuseSampler;

float3 unproject_direction(float2 pos);
float3 unproject(float2 pos, float eye_z);

PixelOut main(PixelIn input)
{
	int x = int(input.ScreenPos.x);
	int y = int(input.ScreenPos.y);
	int2 pos = int2(x,y);
	uint3 texelpos = uint3(pos,0);

	float4 normal_and_z = FaceNormalZTexture.Load(texelpos);
	float3 normal_in_eye = normalize(normal_and_z.xyz);
	float z_in_eye = normal_and_z.w;
	float3 position_in_eye = unproject(float2(x, y) + 0.5f, z_in_eye);

	float3 v = position_in_eye - input.BoxCenter;
	float vx = dot(v, input.BoxX.xyz) * input.BoxX.w;
	float vy = dot(v, input.BoxY.xyz) * input.BoxY.w;
	float vz = dot(v, input.BoxZ.xyz) * input.BoxZ.w;
	float3 position_in_object = float3(vx, vy, vz);

	clip(1 - abs(position_in_object));
	clip(dot(normal_in_eye, input.BoxZ.xyz) - input.Cutoff);

	float4 textureColor = DiffuseTexture.Sample(DiffuseSampler, position_in_object.xy * 0.5 + 0.5);

	PixelOut output;
	output.FragColor = textureColor;
	return output;
}

float3 unproject_direction(float2 pos)
{
//	float field_of_view_y_rad = field_of_view_y_degrees * M_PI / 180.0f;
//	float f = 1.0f / tan(field_of_view_y_rad * 0.5f);
//	float rcp_f = 1.0f / f;
//	float rcp_f_div_aspect = 1.0f / (f / aspect);
	float2 normalized = float2(pos * two_rcp_viewport_size);
#if defined(TOP_DOWN_RENDER_TARGET)
	normalized.x = normalized.x - 1.0f;
	normalized.y = 1.0f - normalized.y;
#else
	normalized -= 1.0f;
#endif
	return float3(normalized.x * rcp_f_div_aspect, normalized.y * rcp_f, 1.0f);
}

float3 unproject(float2 pos, float eye_z)
{
	return unproject_direction(pos) * eye_z;
}

)shaderend"; } }
