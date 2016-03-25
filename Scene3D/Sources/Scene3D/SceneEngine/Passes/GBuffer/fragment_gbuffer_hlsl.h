namespace { const char *fragment_gbuffer_hlsl() { return R"shaderend(

#define DiffuseUV UVMap0
#define BumpMapUV UVMap1
#define SelfIlluminationUV UVMap2
#define SpecularUV UVMap3
#define LightMapUV UVMap4

cbuffer ModelMaterialUniforms
{
	float4 MaterialAmbient;
	float4 MaterialDiffuse;
	float4 MaterialSpecular;
	float MaterialGlossiness;
	float MaterialSpecularLevel;
	uint VectorsPerInstance;
	uint MaterialOffset;
};

struct PixelIn
{
	float4 ScreenPos : SV_Position;
	float3 NormalInEye : NormalInEye;
	float4 PositionInWorld : PositionInWorld;
	float4 PositionInEye : PositionInEye;
#if defined(DIFFUSE_UV)
	float2 UVMap0 : UVMap0;
#endif
#if defined(BUMPMAP_UV)
	float2 UVMap1 : UVMap1;
#endif
#if defined(SI_UV)
	float2 UVMap2 : UVMap2;
#endif
#if defined(SPECULAR_UV)
	float2 UVMap3 : UVMap3;
#endif
#if defined(LIGHTMAP_UV)
	float2 UVMap4 : UVMap4;
#endif
	float ArrayTextureIndex : ArrayTextureIndex;
	float4 SelfIllumination : SelfIllumination;
#if defined(USE_COLORS)
	float4 VertexColor : VertexColor;
#endif
	float4 LightProbeColor : LightProbeColor;
};

struct PixelOut
{
	float4 FragDiffuseColor : SV_Target0;
	float4 FragSpecularColor : SV_Target1;
	float2 FragSpecularLevel : SV_Target2;
	float4 FragSelfIllumination : SV_Target3;
	float4 FragNormal : SV_Target4;
	float4 FragFaceNormal : SV_Target5;
};

#if defined(DIFFUSE_ARRAY)
Texture2DArray DiffuseTexture;
#else
Texture2D DiffuseTexture;
#endif
Texture2D BumpMapTexture;
Texture2D SelfIlluminationTexture;
Texture2D SpecularTexture;
Texture2D LightMapTexture;

SamplerState DiffuseSampler;
SamplerState BumpMapSampler;
SamplerState SelfIlluminationSampler;
SamplerState SpecularSampler;
SamplerState LightMapSampler;

float3 ApplyNormalMap(PixelIn input, bool frontFacing);
float3 DiffuseColor(PixelIn input);
float3 SpecularColor(PixelIn input);
float3 SelfIlluminationColor(PixelIn input, float4 diffuseColor);
float3 LightMapAndProbe(PixelIn input, float4 diffuseColor);

PixelOut main(PixelIn input, bool frontFacing : SV_IsFrontFace)
{
	float3 dFdxPos = ddx(input.PositionInEye.xyz);
	float3 dFdyPos = ddy(input.PositionInEye.xyz);
	float3 faceNormal = normalize(cross(dFdxPos,dFdyPos));

	PixelOut output;
	float3 normalInEyeNormalized = ApplyNormalMap(input, frontFacing);
	output.FragDiffuseColor = float4(DiffuseColor(input), 1);
	output.FragSpecularColor = float4(SpecularColor(input), 1);
	output.FragSpecularLevel = float2(MaterialGlossiness, MaterialSpecularLevel);
	output.FragSelfIllumination = float4(LightMapAndProbe(input, output.FragDiffuseColor) + SelfIlluminationColor(input, output.FragDiffuseColor), 1);
	output.FragNormal = float4(normalInEyeNormalized, 0);
	output.FragFaceNormal = float4(faceNormal, input.PositionInEye.z);

	return output;
}

#if defined(BUMPMAP_UV)

float3x3 cotangent_frame(float3 n, float3 p, float2 uv)
{
	// get edge vectors of the pixel triangle
	float3 dp1 = ddx(p);
	float3 dp2 = ddy(p);
	float2 duv1 = ddx(uv);
	float2 duv2 = ddy(uv);

	// solve the linear system
	float3 dp2perp = cross(n, dp2); // cross(dp2, n);
	float3 dp1perp = cross(dp1, n); // cross(n, dp1);
	float3 t = dp2perp * duv1.x + dp1perp * duv2.x;
	float3 b = dp2perp * duv1.y + dp1perp * duv2.y;

	// construct a scale-invariant frame
	float invmax = rsqrt(max(dot(t,t), dot(b,b)));
	return transpose(float3x3(t * invmax, b * invmax, n));
}

float3 ApplyNormalMap(PixelIn input, bool frontFacing)
{
	#define WITH_NORMALMAP_UNSIGNED
	#define WITH_NORMALMAP_GREEN_UP
	#define WITH_NORMALMAP_2CHANNEL

	float3 interpolatedNormal = normalize(frontFacing ? input.NormalInEye : -input.NormalInEye);

	float3 map = BumpMapTexture.Sample(BumpMapSampler, input.BumpMapUV).xyz;
	#if defined(WITH_NORMALMAP_UNSIGNED)
	map = map * 255./127. - 128./127.; // Math so "odd" because 0.5 cannot be precisely described in an unsigned format
	#endif
	#if defined(WITH_NORMALMAP_2CHANNEL)
	map.z = sqrt(1 - dot(map.xy, map.xy));
	#endif
	#if defined(WITH_NORMALMAP_GREEN_UP)
	map.y = -map.y;
	#endif

	float3x3 tbn = cotangent_frame(interpolatedNormal, input.PositionInEye.xyz, input.BumpMapUV);
	float3 bumpedNormal = normalize(mul(tbn, map));

	// Mix between the original normal vector and the bumped normal vector according to the material properties
	//float bumpAmount = 0.30; // Hardcoded to the 3ds max default for now
	//return normalize(lerp(frontNormal, bumpedNormal, bumpAmount));

	return bumpedNormal;
}

#else

float3 ApplyNormalMap(PixelIn input, bool frontFacing)
{
	float3 frontNormal = frontFacing ? input.NormalInEye : -input.NormalInEye;
	return normalize(frontNormal);
}

#endif

#if defined(DIFFUSE_UV)

float3 DiffuseColor(PixelIn input)
{
#if defined(DIFFUSE_ARRAY)
	float4 color = DiffuseTexture.Sample(DiffuseSampler, float3(input.DiffuseUV, input.ArrayTextureIndex));
#else
	float4 color = DiffuseTexture.Sample(DiffuseSampler, input.DiffuseUV);
#endif
	if (color.a < 0.5)
		discard;
	return color.rgb;
}

#else

float3 DiffuseColor(PixelIn input)
{
	return MaterialDiffuse.xyz;
}

#endif

#if defined(SPECULAR_UV)

float3 SpecularColor(PixelIn input)
{
	return SpecularTexture.Sample(SpecularSampler, input.SpecularUV).rgb;
}

#else

float3 SpecularColor(PixelIn input)
{
	return MaterialSpecular.xyz;
}

#endif

#if defined(SI_UV)

float3 SelfIlluminationColor(PixelIn input, float4 diffuseColor)
{
#if defined(USE_COLORS)
	float3 siColor = input.SelfIllumination.rgb + input.VertexColor.rgb * diffuseColor.rgb;
	return lerp(siColor, SelfIlluminationTexture.Sample(SelfIlluminationSampler, input.SelfIlluminationUV).rgb, input.SelfIllumination.w) * 2;
#else
	return lerp(input.SelfIllumination.rgb, SelfIlluminationTexture.Sample(SelfIlluminationSampler, input.SelfIlluminationUV).rgb, input.SelfIllumination.w) * 2;
#endif
}

#else

float3 SelfIlluminationColor(PixelIn input, float4 diffuseColor)
{
#if defined(USE_COLORS)
	return input.SelfIllumination.rgb * 2 + input.VertexColor.rgb * diffuseColor.rgb;
#else
	return input.SelfIllumination.rgb * 2;
#endif
}

#endif

float3 LightMapAndProbe(PixelIn input, float4 diffuseColor)
{
#if defined(LIGHTMAP_UV)
	return diffuseColor.rgb * (input.LightProbeColor.rgb + LightMapTexture.Sample(LightMapSampler, input.LightMapUV).rgb);
#else
	return diffuseColor.rgb * input.LightProbeColor.rgb;
#endif
}

)shaderend"; } }
