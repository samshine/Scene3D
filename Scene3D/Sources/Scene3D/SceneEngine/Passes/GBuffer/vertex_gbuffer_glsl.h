namespace { const char *vertex_gbuffer_glsl() { return R"shaderend(

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

in vec4 AttrPositionInObject;
in vec3 AttrNormal;
in vec3 AttrBitangent;
in vec3 AttrTangent;
#if defined(USE_BONES)
in vec4 AttrBoneWeights;
in ivec4 AttrBoneSelectors;
#endif
#if defined(USE_COLORS)
in vec4 AttrColor;
#endif
#if defined(DIFFUSE_UV)
in vec2 AttrUVMapA;
#endif
#if defined(BUMPMAP_UV)
in vec2 AttrUVMapB;
#endif
#if defined(SI_UV)
in vec2 AttrUVMapC;
#endif
#if defined(SPECULAR_UV)
in vec2 AttrUVMapD;
#endif


out vec3 NormalInEye;
out vec3 BitangentInEye;
out vec3 TangentInEye;
out vec4 PositionInWorld;
out vec4 PositionInEye;
#if defined(DIFFUSE_UV)
out vec2 UVMap0;
#endif
#if defined(BUMPMAP_UV)
out vec2 UVMap1;
#endif
#if defined(SI_UV)
out vec2 UVMap2;
#endif
#if defined(SPECULAR_UV)
out vec2 UVMap3;
#endif
out float ArrayTextureIndex;
out vec4 SelfIllumination;
#if defined(USE_COLORS)
out vec4 VertexColor;
#endif
out vec4 LightProbeColor;

uniform sampler2D InstanceVectors;

struct BonesResult
{
	vec3 Normal;
	vec3 Tangent;
	vec3 Bitangent;
	vec4 PositionInObject;
};

BonesResult ApplyBones(int instanceBonesOffset);
mat4 GetBoneTransform(int boneIndex, int instanceBonesOffset);
ivec2 GetTexelPosition(int index);
int GetVectorsOffset(int instanceId);

mat3 loadMat3(int offset)
{
	return transpose(mat3(
		texelFetch(InstanceVectors, GetTexelPosition(offset + 0), 0).xyz,
		texelFetch(InstanceVectors, GetTexelPosition(offset + 1), 0).xyz,
		texelFetch(InstanceVectors, GetTexelPosition(offset + 2), 0).xyz));
}

mat4 loadMat4(int offset)
{
	return transpose(mat4(
		texelFetch(InstanceVectors, GetTexelPosition(offset + 0), 0),
		texelFetch(InstanceVectors, GetTexelPosition(offset + 1), 0),
		texelFetch(InstanceVectors, GetTexelPosition(offset + 2), 0),
		texelFetch(InstanceVectors, GetTexelPosition(offset + 3), 0)));
}

mat4x3 loadMat4x3(int offset)
{
	return 
		transpose(mat3x4(
			texelFetch(InstanceVectors, GetTexelPosition(offset + 0), 0),
			texelFetch(InstanceVectors, GetTexelPosition(offset + 1), 0),
			texelFetch(InstanceVectors, GetTexelPosition(offset + 2), 0)));
}

void main()
{
	int vectorsOffset = GetVectorsOffset(gl_InstanceID);

	mat3 ObjectNormalToEye = loadMat3(vectorsOffset);
	mat4 ObjectToWorld = loadMat4(vectorsOffset + 3);
	mat4 WorldToEye = loadMat4(vectorsOffset + 7);
	mat4 EyeToProjection = loadMat4(vectorsOffset + 11);
	vec4 SelfIlluminationData = texelFetch(InstanceVectors, GetTexelPosition(vectorsOffset + MaterialOffset), 0);
	vec4 MaterialInstanceData = texelFetch(InstanceVectors, GetTexelPosition(vectorsOffset + MaterialOffset + 1), 0);
	vec4 LightProbeColorData = texelFetch(InstanceVectors, GetTexelPosition(vectorsOffset + 15), 0);

	BonesResult bonesResult = ApplyBones(vectorsOffset + 16);
	mat3 TangentObjectToEye = mat3(WorldToEye * ObjectToWorld);
	NormalInEye = normalize(ObjectNormalToEye * bonesResult.Normal);
	TangentInEye = normalize(TangentObjectToEye * bonesResult.Tangent);
	BitangentInEye = normalize(TangentObjectToEye * bonesResult.Bitangent);
	//BitangentInEye = normalize(cross(output.NormalInEye, output.TangentInEye));
#if defined(DIFFUSE_UV)
	mat4x3 UVTextureMatrix0 = loadMat4x3(vectorsOffset + MaterialOffset + 2);
	UVMap0 = (UVTextureMatrix0 * vec4(AttrUVMapA, 0, 1)).xy;
#endif
#if defined(BUMPMAP_UV)
	mat4x3 UVTextureMatrix1 = loadMat4x3(vectorsOffset + MaterialOffset + 5);
	UVMap1 = (UVTextureMatrix1 * vec4(AttrUVMapB, 0, 1)).xy;
#endif
#if defined(SI_UV)
	mat4x3 UVTextureMatrix2 = loadMat4x3(vectorsOffset + MaterialOffset + 8);
	UVMap2 = (UVTextureMatrix2 * vec4(AttrUVMapC, 0, 1)).xy;
#endif
#if defined(SPECULAR_UV)
	mat4x3 UVTextureMatrix3 = loadMat4x3(vectorsOffset + MaterialOffset + 11);
	UVMap3 = (UVTextureMatrix3 * vec4(AttrUVMapD, 0, 1)).xy;
#endif
	PositionInWorld = ObjectToWorld * bonesResult.PositionInObject;
	PositionInEye = WorldToEye * PositionInWorld;
	gl_Position = EyeToProjection * PositionInEye;
	ArrayTextureIndex = MaterialInstanceData.x;
	SelfIllumination = SelfIlluminationData;
#if defined(USE_COLORS)
	VertexColor = AttrColor;
#endif
	LightProbeColor = LightProbeColorData;
}

#if defined(USE_BONES)
BonesResult ApplyBones(int instanceBonesOffset)
{
	BonesResult result;
	if (AttrBoneWeights.x != 0.0 || AttrBoneWeights.y != 0.0 || AttrBoneWeights.z != 0.0 || AttrBoneWeights.w != 0.0)
	{
		result.PositionInObject = vec4(0,0,0,0);
		result.Normal = vec3(0,0,0);
		result.Tangent = vec3(0,0,0);
		result.Bitangent = vec3(0,0,0);

		// We use low precision input for our bone weights. Rescale so the sum still is 1.0
		float totalWeight = AttrBoneWeights.x + AttrBoneWeights.y + AttrBoneWeights.z + AttrBoneWeights.w;
		float weightMultiplier = 1 / totalWeight;
		vec4 BoneWeights = AttrBoneWeights * weightMultiplier;

		if (BoneWeights.x != 0.0)
		{
			mat4 transform = GetBoneTransform(AttrBoneSelectors.x, instanceBonesOffset);
			mat3 rotation = mat3(transform);
			result.PositionInObject += (transform * AttrPositionInObject) * BoneWeights.x;
			result.Normal += (rotation * AttrNormal) * BoneWeights.x;
			result.Tangent += (rotation * AttrTangent) * BoneWeights.x;
			result.Bitangent += (rotation * AttrBitangent) * BoneWeights.x;
		}

		if (BoneWeights.y != 0.0)
		{
			mat4 transform = GetBoneTransform(AttrBoneSelectors.y, instanceBonesOffset);
			mat3 rotation = mat3(transform);
			result.PositionInObject += (transform * AttrPositionInObject) * BoneWeights.y;
			result.Normal += (rotation * AttrNormal) * BoneWeights.y;
			result.Tangent += (rotation * AttrTangent) * BoneWeights.y;
			result.Bitangent += (rotation * AttrBitangent) * BoneWeights.y;
		}

		if (BoneWeights.z != 0.0)
		{
			mat4 transform = GetBoneTransform(AttrBoneSelectors.z, instanceBonesOffset);
			mat3 rotation = mat3(transform);
			result.PositionInObject += (transform * AttrPositionInObject) * BoneWeights.z;
			result.Normal += (rotation * AttrNormal) * BoneWeights.z;
			result.Tangent += (rotation * AttrTangent) * BoneWeights.z;
			result.Bitangent += (rotation * AttrBitangent) * BoneWeights.z;
		}

		if (BoneWeights.w != 0.0)
		{
			mat4 transform = GetBoneTransform(AttrBoneSelectors.w, instanceBonesOffset);
			mat3 rotation = mat3(transform);
			result.PositionInObject += (transform * AttrPositionInObject) * BoneWeights.w;
			result.Normal += (rotation * AttrNormal) * BoneWeights.w;
			result.Tangent += (rotation * AttrTangent) * BoneWeights.w;
			result.Bitangent += (rotation * AttrBitangent) * BoneWeights.w;
		}

		result.PositionInObject.w = 1.0; // For numerical stability
	}
	else
	{
		result.PositionInObject = AttrPositionInObject;
		result.Normal = AttrNormal;
		result.Tangent = AttrTangent;
		result.Bitangent = AttrBitangent;
	}
	return result;
}
#else
BonesResult ApplyBones(int instanceBonesOffset)
{
	BonesResult result;
	result.PositionInObject = AttrPositionInObject;
	result.Normal = AttrNormal;
	result.Tangent = AttrTangent;
	result.Bitangent = AttrBitangent;
	return result;
}
#endif

mat4 GetBoneTransform(int boneIndex, int instanceBonesOffset)
{
	vec4 row0 = texelFetch(InstanceVectors, GetTexelPosition(instanceBonesOffset + boneIndex * 3 + 0), 0);
	vec4 row1 = texelFetch(InstanceVectors, GetTexelPosition(instanceBonesOffset + boneIndex * 3 + 1), 0);
	vec4 row2 = texelFetch(InstanceVectors, GetTexelPosition(instanceBonesOffset + boneIndex * 3 + 2), 0);
	vec4 row3 = vec4(0,0,0,1);
	return transpose(mat4(row0, row1, row2, row3));
}

ivec2 GetTexelPosition(int index)
{
	int width = textureSize(InstanceVectors, 0).x;
	return ivec2(index % width, index / width);
}

int GetVectorsOffset(int instanceId)
{
	return instanceId * VectorsPerInstance;
}

)shaderend"; } }
