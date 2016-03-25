namespace { const char *fragment_gbuffer_glsl() { return R"shaderend(

#define DiffuseUV UVMap0
#define BumpMapUV UVMap1
#define SelfIlluminationUV UVMap2
#define SpecularUV UVMap3

layout(std140) uniform ModelMaterialUniforms
{
	vec4 MaterialAmbient;
	vec4 MaterialDiffuse;
	vec4 MaterialSpecular;
	float MaterialGlossiness;
	float MaterialSpecularLevel;
	int VectorsPerInstance;
	int MaterialOffset;
};

in vec3 NormalInEye;
in vec4 PositionInWorld;
in vec4 PositionInEye;
#if defined(DIFFUSE_UV)
in vec2 UVMap0;
#endif
#if defined(BUMPMAP_UV)
in vec2 UVMap1;
#endif
#if defined(SI_UV)
in vec2 UVMap2;
#endif
#if defined(SPECULAR_UV)
in vec2 UVMap3;
#endif
in float ArrayTextureIndex;
in vec4 SelfIllumination;
#if defined(USE_COLORS)
in vec4 VertexColor;
#endif
in vec4 LightProbeColor;

out vec4 FragDiffuseColor;
out vec4 FragSpecularColor;
out vec2 FragSpecularLevel;
out vec4 FragSelfIllumination;
out vec4 FragNormal;
out vec4 FragFaceNormal;

#if defined(DIFFUSE_ARRAY)
uniform sampler2DArray DiffuseTexture;
#else
uniform sampler2D DiffuseTexture;
#endif
uniform sampler2D BumpMapTexture;
uniform sampler2D SelfIlluminationTexture;
uniform sampler2D SpecularTexture;

vec3 ApplyNormalMap(bool frontFacing);
vec3 DiffuseColor();
vec3 SpecularColor();
vec3 SelfIlluminationColor(vec4 diffuseColor);

void main()
{
	vec3 dFdxPos = dFdx(PositionInEye.xyz);
	vec3 dFdyPos = dFdy(PositionInEye.xyz);
	vec3 faceNormal = normalize(cross(dFdxPos,dFdyPos));

	vec3 normalInEyeNormalized = ApplyNormalMap(gl_FrontFacing);
	FragDiffuseColor = vec4(DiffuseColor(), 1);
	FragSpecularColor = vec4(SpecularColor(), 1);
	FragSpecularLevel = vec2(MaterialGlossiness, MaterialSpecularLevel);
	FragSelfIllumination = vec4(FragDiffuseColor.rgb * LightProbeColor.rgb + SelfIlluminationColor(FragDiffuseColor), 1);
	FragNormal = vec4(normalInEyeNormalized, 0);
	FragFaceNormal = vec4(faceNormal, PositionInEye.z);
}

#if defined(BUMPMAP_UV)

mat3 cotangent_frame(vec3 n, vec3 p, vec2 uv)
{
	// get edge vectors of the pixel triangle
	vec3 dp1 = dFdx(p);
	vec3 dp2 = dFdy(p);
	vec2 duv1 = dFdx(uv);
	vec2 duv2 = dFdy(uv);

	// solve the linear system
	vec3 dp2perp = cross(n, dp2); // cross(dp2, n);
	vec3 dp1perp = cross(dp1, n); // cross(n, dp1);
	vec3 t = dp2perp * duv1.x + dp1perp * duv2.x;
	vec3 b = dp2perp * duv1.y + dp1perp * duv2.y;

	// construct a scale-invariant frame
	float invmax = inversesqrt(max(dot(t,t), dot(b,b)));
	return mat3(t * invmax, b * invmax, n);
}

vec3 ApplyNormalMap(bool frontFacing)
{
	#define WITH_NORMALMAP_UNSIGNED
	#define WITH_NORMALMAP_GREEN_UP
	#define WITH_NORMALMAP_2CHANNEL

	vec3 interpolatedNormal = normalize(frontFacing ? NormalInEye : -NormalInEye);

	vec3 map = texture(BumpMapTexture, BumpMapUV).xyz;
	#if defined(WITH_NORMALMAP_UNSIGNED)
	map = map * 255./127. - 128./127.; // Math so "odd" because 0.5 cannot be precisely described in an unsigned format
	#endif
	#if defined(WITH_NORMALMAP_2CHANNEL)
	map.z = sqrt(1 - dot(map.xy, map.xy));
	#endif
	#if defined(WITH_NORMALMAP_GREEN_UP)
	map.y = -map.y;
	#endif

	mat3 tbn = cotangent_frame(interpolatedNormal, PositionInEye.xyz, BumpMapUV);
	vec3 bumpedNormal = normalize(tbn * map);

	// Mix between the original normal vector and the bumped normal vector according to the material properties
	//float bumpAmount = 0.30; // Hardcoded to the 3ds max default for now
	//return normalize(lerp(frontNormal, bumpedNormal, bumpAmount));

	return bumpedNormal;
}

#else

vec3 ApplyNormalMap(bool frontFacing)
{
	vec3 frontNormal = frontFacing ? NormalInEye : -NormalInEye;
	return normalize(frontNormal);
}

#endif

#if defined(DIFFUSE_UV)

vec3 DiffuseColor()
{
#if defined(DIFFUSE_ARRAY)
	vec4 color = texture(DiffuseTexture, DiffuseUV, ArrayTextureIndex);
#else
	vec4 color = texture(DiffuseTexture, DiffuseUV);
#endif
	if (color.a < 0.5)
		discard;
	return color.rgb;
}

#else

vec3 DiffuseColor()
{
	return MaterialDiffuse.xyz;
}

#endif

#if defined(SPECULAR_UV)

vec3 SpecularColor()
{
	return texture(SpecularTexture, SpecularUV).rgb;
}

#else

vec3 SpecularColor()
{
	return MaterialSpecular.xyz;
}

#endif

#if defined(SI_UV)

vec3 SelfIlluminationColor(vec4 diffuseColor)
{
#if defined(USE_COLORS)
	vec3 siColor = SelfIllumination.rgb + VertexColor.rgb * diffuseColor.rgb;
	return mix(siColor, texture(SelfIlluminationTexture, SelfIlluminationUV).rgb, SelfIllumination.w);
#else
	return mix(SelfIllumination.rgb, texture(SelfIlluminationTexture, SelfIlluminationUV).rgb, SelfIllumination.w);
#endif
}

#else

vec3 SelfIlluminationColor(vec4 diffuseColor)
{
#if defined(USE_COLORS)
	return SelfIllumination.rgb + VertexColor.rgb * diffuseColor.rgb;
#else
	return SelfIllumination.rgb;
#endif
}

#endif

)shaderend"; } }
