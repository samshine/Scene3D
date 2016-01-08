namespace { const char *vertex_shadow_glsl() { return R"shaderend(

layout(std140) uniform ModelMaterialUniforms
{
	vec4 MaterialAmbient;
	vec4 MaterialDiffuse;
	vec4 MaterialSpecular;
	float MaterialGlossiness;
	float MaterialSpecularLevel;
	int ModelIndex;
	int VectorsPerInstance;
	int MaterialOffset;
};

in vec4 AttrPositionInObject;
#if defined(USE_BONES)
in vec4 AttrBoneWeights;
in ivec4 AttrBoneSelectors;
#endif

out vec4 PositionInEye;

uniform sampler2D InstanceOffsets;
uniform sampler2D InstanceVectors;


vec4 ApplyBones(int instanceBonesOffset);
mat4 GetBoneTransform(int boneIndex, int instanceBonesOffset);
mat4x2 GetBoneDualQuaternion(mat4x2 currentDual, int boneIndex, int instanceBonesOffset);
ivec2 GetTexelPosition(int index);
int GetVectorsOffset(int index);

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

void main()
{
	int vectorsOffset = GetVectorsOffset(ModelIndex);

	mat4 ObjectToWorld = loadMat4(vectorsOffset + gl_InstanceID * VectorsPerInstance + 3);
	mat4 WorldToEye = loadMat4(vectorsOffset + gl_InstanceID * VectorsPerInstance + 7);
	mat4 EyeToProjection = loadMat4(vectorsOffset + gl_InstanceID * VectorsPerInstance + 11);

	vec4 positionInObject = ApplyBones(vectorsOffset + gl_InstanceID * VectorsPerInstance + 16);
	PositionInEye = WorldToEye * ObjectToWorld * positionInObject;
	gl_Position = EyeToProjection * PositionInEye;
}

#if defined(USE_BONES) && defined(USE_DUALQUATERNION)
vec4 ApplyBones(int instanceBonesOffset)
{
	vec4 resultPositionInObject;
	if (AttrBoneWeights.x != 0.0 || AttrBoneWeights.y != 0.0 || AttrBoneWeights.z != 0.0 || AttrBoneWeights.w != 0.0)
	{
		// We use low precision input for our bone weights. Rescale so the sum still is 1.0
		float totalWeight = AttrBoneWeights.x + AttrBoneWeights.y + AttrBoneWeights.z + AttrBoneWeights.w;
		float weightMultiplier = 1 / totalWeight;
		vec4 BoneWeights = AttrBoneWeights * weightMultiplier;

		// For details, read the paper "Geometric Skinning with Dual Skinning" by L. Kavan et al.
		mat4x2 dual = mat4x2(0);
		dual  = BoneWeights.x * GetBoneDualQuaternion(dual, AttrBoneSelectors.x, instanceBonesOffset);
		dual += BoneWeights.y * GetBoneDualQuaternion(dual, AttrBoneSelectors.y, instanceBonesOffset);
		dual += BoneWeights.z * GetBoneDualQuaternion(dual, AttrBoneSelectors.z, instanceBonesOffset);
		dual += BoneWeights.w * GetBoneDualQuaternion(dual, AttrBoneSelectors.w, instanceBonesOffset);
		dual /= length(dual[0]);

		resultPositionInObject.xyz = AttrPositionInObject.xyz + 2.0 * cross(dual[0].xyz, cross(dual[0].xyz, AttrPositionInObject.xyz) + dual[0].w * AttrPositionInObject.xyz);
		resultPositionInObject.xyz += 2.0 * (dual[0].w * dual[1].xyz - dual[1].w * dual[0].xyz + cross(dual[0].xyz, dual[1].xyz));
		resultPositionInObject.w = 1;
	}
	else
	{
		resultPositionInObject = AttrPositionInObject;
	}
	return resultPositionInObject;
}
#elif defined(USE_BONES)
vec4 ApplyBones(int instanceBonesOffset)
{
	vec4 resultPositionInObject;
	if (AttrBoneWeights.x != 0.0 || AttrBoneWeights.y != 0.0 || AttrBoneWeights.z != 0.0 || AttrBoneWeights.w != 0.0)
	{
		resultPositionInObject = vec4(0,0,0,0);

		// We use low precision input for our bone weights. Rescale so the sum still is 1.0
		float totalWeight = AttrBoneWeights.x + AttrBoneWeights.y + AttrBoneWeights.z + AttrBoneWeights.w;
		float weightMultiplier = 1 / totalWeight;
		vec4 BoneWeights = AttrBoneWeights * weightMultiplier;

		if (BoneWeights.x != 0.0)
		{
			mat4 transform = GetBoneTransform(AttrBoneSelectors.x, instanceBonesOffset);
			mat3 rotation = mat3(transform);
			resultPositionInObject += (transform * AttrPositionInObject) * BoneWeights.x;
		}

		if (BoneWeights.y != 0.0)
		{
			mat4 transform = GetBoneTransform(AttrBoneSelectors.y, instanceBonesOffset);
			mat3 rotation = mat3(transform);
			resultPositionInObject += (transform * AttrPositionInObject) * BoneWeights.y;
		}

		if (BoneWeights.z != 0.0)
		{
			mat4 transform = GetBoneTransform(AttrBoneSelectors.z, instanceBonesOffset);
			mat3 rotation = mat3(transform);
			resultPositionInObject += (transform * AttrPositionInObject) * BoneWeights.z;
		}

		if (BoneWeights.w != 0.0)
		{
			mat4 transform = GetBoneTransform(AttrBoneSelectors.w, instanceBonesOffset);
			mat3 rotation = mat3(transform);
			resultPositionInObject += (transform * AttrPositionInObject) * BoneWeights.w;
		}

		resultPositionInObject.w = 1.0; // For numerical stability
	}
	else
	{
		resultPositionInObject = AttrPositionInObject;
	}
	return resultPositionInObject;
}
#else
vec4 ApplyBones(int instanceBonesOffset)
{
	return AttrPositionInObject;
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
	int width = textureSize(InstanceVectors, 0).x;
	return ivec2(index % width, index / width);
}

int GetVectorsOffset(int instanceId)
{
	int width = textureSize(InstanceOffsets, 0).x;
	int modelOffset = int(texelFetch(InstanceOffsets, ivec2(ModelIndex % width, ModelIndex / width), 0).x);
	return modelOffset + instanceId * VectorsPerInstance;
}

)shaderend"; } }
