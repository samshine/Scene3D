namespace { const char *vertex_transparency_glsl() { return R"shaderend(

layout(std140) uniform ModelRenderUniforms
{
	int BaseVectorOffset;
};

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
#if defined(USE_BONES)
in vec4 AttrBoneWeights;
in ivec4 AttrBoneSelectors;
#endif
//in vec4 AttrColor;
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

uniform sampler2D InstanceOffsets;
uniform sampler2D InstanceVectors;

struct BonesResult
{
	vec3 Normal;
	vec4 PositionInObject;
};

BonesResult ApplyBones(int instanceBonesOffset);
mat4 GetBoneTransform(int boneIndex, int instanceBonesOffset);
mat4x2 GetBoneDualQuaternion(mat4x2 currentDual, int boneIndex, int instanceBonesOffset);
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

	BonesResult bonesResult = ApplyBones(vectorsOffset + 15);
	NormalInEye = normalize(ObjectNormalToEye * bonesResult.Normal);
#if defined(DIFFUSE_UV)
	UVMap0 = AttrUVMapA;
#endif
#if defined(BUMPMAP_UV)
	UVMap1 = AttrUVMapB;
#endif
#if defined(SI_UV)
	UVMap2 = AttrUVMapC;
#endif
#if defined(SPECULAR_UV)
	UVMap3 = AttrUVMapD;
#endif
	PositionInWorld = ObjectToWorld * bonesResult.PositionInObject;
	PositionInEye = WorldToEye * PositionInWorld;
	gl_Position = EyeToProjection * PositionInEye;
	ArrayTextureIndex = MaterialInstanceData.x;
	SelfIllumination = SelfIlluminationData;
}

#if defined(USE_BONES) && defined(USE_DUALQUATERNION)
BonesResult ApplyBones(int instanceBonesOffset)
{
	BonesResult result;
	if (AttrBoneWeights.x != 0.0 || AttrBoneWeights.y != 0.0 || AttrBoneWeights.z != 0.0 || AttrBoneWeights.w != 0.0)
	{
		// We use low precision input for our bone weights. Rescale so the sum still is 1.0
		float totalWeight = AttrBoneWeights.x + AttrBoneWeights.y + AttrBoneWeights.z + AttrBoneWeights.w;
		float weightMultiplier = 1 / totalWeight;
		vec4 BoneWeights = AttrBoneWeights * weightMultiplier;

		// For details, read the paper "Geometric Skinning with Dual Skinning" by L. Kavan et al.
		mat4x2 dual = (mat4x2)0;
		dual  = BoneWeights.x * GetBoneDualQuaternion(dual, AttrBoneSelectors.x, instanceBonesOffset);
		dual += BoneWeights.y * GetBoneDualQuaternion(dual, AttrBoneSelectors.y, instanceBonesOffset);
		dual += BoneWeights.z * GetBoneDualQuaternion(dual, AttrBoneSelectors.z, instanceBonesOffset);
		dual += BoneWeights.w * GetBoneDualQuaternion(dual, AttrBoneSelectors.w, instanceBonesOffset);
		dual /= length(dual[0]);

		result.PositionInObject.xyz = AttrPositionInObject.xyz + 2.0 * cross(dual[0].xyz, cross(dual[0].xyz, AttrPositionInObject.xyz) + dual[0].w * AttrPositionInObject.xyz);
		result.PositionInObject.xyz += 2.0 * (dual[0].w * dual[1].xyz - dual[1].w * dual[0].xyz + cross(dual[0].xyz, dual[1].xyz));
		result.PositionInObject.w = 1;

		result.Normal = AttrNormal + 2.0 * cross(dual[0].xyz, cross(dual[0].xyz, AttrNormal) + dual[0].w * AttrNormal);
	}
	else
	{
		result.PositionInObject = AttrPositionInObject;
		result.Normal = AttrNormal;
	}
	return result;
}
#elif defined(USE_BONES)
BonesResult ApplyBones(int instanceBonesOffset)
{
	BonesResult result;
	if (AttrBoneWeights.x != 0.0 || AttrBoneWeights.y != 0.0 || AttrBoneWeights.z != 0.0 || AttrBoneWeights.w != 0.0)
	{
		// We use low precision input for our bone weights. Rescale so the sum still is 1.0
		float totalWeight = AttrBoneWeights.x + AttrBoneWeights.y + AttrBoneWeights.z + AttrBoneWeights.w;
		float weightMultiplier = 1 / totalWeight;
		vec4 BoneWeights = AttrBoneWeights * weightMultiplier;

		result.PositionInObject = vec4(0,0,0,0);
		result.Normal = vec3(0,0,0);

		if (BoneWeights.x != 0.0)
		{
			mat4 transform = GetBoneTransform(AttrBoneSelectors.x, instanceBonesOffset);
			mat3 rotation = mat3(transform);
			result.PositionInObject += (transform * AttrPositionInObject) * BoneWeights.x;
			result.Normal += (rotation * AttrNormal) * BoneWeights.x;
		}

		if (BoneWeights.y != 0.0)
		{
			mat4 transform = GetBoneTransform(AttrBoneSelectors.y, instanceBonesOffset);
			mat3 rotation = mat3(transform);
			result.PositionInObject += (transform * AttrPositionInObject) * BoneWeights.y;
			result.Normal += (rotation * AttrNormal) * BoneWeights.y;
		}

		if (BoneWeights.z != 0.0)
		{
			mat4 transform = GetBoneTransform(AttrBoneSelectors.z, instanceBonesOffset);
			mat3 rotation = mat3(transform);
			result.PositionInObject += (transform * AttrPositionInObject) * BoneWeights.z;
			result.Normal += (rotation * AttrNormal) * BoneWeights.z;
		}

		if (BoneWeights.w != 0.0)
		{
			mat4 transform = GetBoneTransform(AttrBoneSelectors.w, instanceBonesOffset);
			mat3 rotation = mat3(transform);
			result.PositionInObject += (transform * AttrPositionInObject) * BoneWeights.w;
			result.Normal += (rotation * AttrNormal) * BoneWeights.w;
		}

		result.PositionInObject.w = 1.0; // For numerical stability
	}
	else
	{
		result.PositionInObject = AttrPositionInObject;
		result.Normal = AttrNormal;
	}
	return result;
}
#else
BonesResult ApplyBones(int instanceBonesOffset)
{
	BonesResult result;
	result.PositionInObject = AttrPositionInObject;
	result.Normal = AttrNormal;
	return result;
}
#endif

#if !defined(USE_DUALQUATERNION)
mat4 GetBoneTransform(int boneIndex, int instanceBonesOffset)
{
	vec4 row0 = texelFetch(InstanceVectors, GetTexelPosition(instanceBonesOffset + boneIndex * 3 + 0), 0);
	vec4 row1 = texelFetch(InstanceVectors, GetTexelPosition(instanceBonesOffset + boneIndex * 3 + 1), 0);
	vec4 row2 = texelFetch(InstanceVectors, GetTexelPosition(instanceBonesOffset + boneIndex * 3 + 2), 0);
	vec4 row3 = vec4(0,0,0,1);
	return transpose(mat4(row0, row1, row2, row3));
}
#else
mat4x2 GetBoneDualQuaternion(mat4x2 currentDual, int boneIndex, int instanceBonesOffset)
{
	vec4 row0 = texelFetch(InstanceVectors, GetTexelPosition(instanceBonesOffset + boneIndex * 2 + 0), 0);
	vec4 row1 = texelFetch(InstanceVectors, GetTexelPosition(instanceBonesOffset + boneIndex * 2 + 1), 0);
	mat4x2 newDual = transpose(mat4x2(row0, row1));
	if (dot(currentDual[0], newDual[0]) < 0)
		return -newDual;
	else
		return newDual;
}
#endif

ivec2 GetTexelPosition(int index)
{
	int width = int(textureSize(InstanceVectors, 0).x);
	return ivec2(index % width, index / width);
}

int GetVectorsOffset(int instanceId)
{
	return BaseVectorOffset + instanceId * VectorsPerInstance;
}

)shaderend"; } }
