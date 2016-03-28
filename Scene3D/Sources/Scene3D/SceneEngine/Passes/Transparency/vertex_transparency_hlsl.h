namespace { const char *vertex_transparency_hlsl() { return R"shaderend(

cbuffer ModelRenderUniforms
{
	uint BaseVectorOffset;
};

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

struct VertexIn
{
	float4 AttrPositionInObject : AttrPositionInObject;
	float3 AttrNormal : AttrNormal;
#if defined(USE_BONES)
	float4 AttrBoneWeights : AttrBoneWeights;
	uint4 AttrBoneSelectors : AttrBoneSelectors;
#endif
	//float4 AttrColor : AttrColor;
#if defined(DIFFUSE_UV)
	float2 AttrUVMap0 : AttrUVMapA;
#endif
#if defined(BUMPMAP_UV)
	float2 AttrUVMap1 : AttrUVMapB;
#endif
#if defined(SI_UV)
	float2 AttrUVMap2 : AttrUVMapC;
#endif
#if defined(SPECULAR_UV)
	float2 AttrUVMap3 : AttrUVMapD;
#endif
};

struct VertexOut
{
	float4 PositionInProjection : SV_Position;
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
	float ArrayTextureIndex : ArrayTextureIndex;
	float4 SelfIllumination : SelfIllumination;
};

Texture2D InstanceOffsets;
Texture2D InstanceVectors;

struct BonesResult
{
	float3 Normal;
	float4 PositionInObject;
};

BonesResult ApplyBones(VertexIn input, uint instanceBonesOffset);
float4x4 GetBoneTransform(uint boneIndex, uint instanceBonesOffset);
float2x4 GetBoneDualQuaternion(float2x4 currentDual, uint boneIndex, uint instanceBonesOffset);
int3 GetTexelPosition(uint index);
uint GetVectorsOffset(uint instanceId);

float3x3 loadMat3(uint offset)
{
	return float3x3(
		InstanceVectors.Load(GetTexelPosition(offset + 0)).xyz,
		InstanceVectors.Load(GetTexelPosition(offset + 1)).xyz,
		InstanceVectors.Load(GetTexelPosition(offset + 2)).xyz);
}

float4x4 loadMat4(uint offset)
{
	return float4x4(
		InstanceVectors.Load(GetTexelPosition(offset + 0)),
		InstanceVectors.Load(GetTexelPosition(offset + 1)),
		InstanceVectors.Load(GetTexelPosition(offset + 2)),
		InstanceVectors.Load(GetTexelPosition(offset + 3)));
}

float3x4 loadMat3x4(uint offset)
{
	return 
		float3x4(
			InstanceVectors.Load(GetTexelPosition(offset + 0)),
			InstanceVectors.Load(GetTexelPosition(offset + 1)),
			InstanceVectors.Load(GetTexelPosition(offset + 2)));
}

VertexOut main(VertexIn input, uint instanceId : SV_InstanceId)
{
	uint vectorsOffset = GetVectorsOffset(instanceId);

	float3x3 ObjectNormalToEye = loadMat3(vectorsOffset);
	float4x4 ObjectToWorld = loadMat4(vectorsOffset + 3);
	float4x4 WorldToEye = loadMat4(vectorsOffset + 7);
	float4x4 EyeToProjection = loadMat4(vectorsOffset + 11);
	float4 SelfIlluminationData = InstanceVectors.Load(GetTexelPosition(vectorsOffset + MaterialOffset));
	float4 MaterialInstanceData = InstanceVectors.Load(GetTexelPosition(vectorsOffset + MaterialOffset + 1));

	VertexOut output;
	BonesResult bonesResult = ApplyBones(input, vectorsOffset + 15);
	output.NormalInEye = normalize(mul(ObjectNormalToEye, bonesResult.Normal));
#if defined(DIFFUSE_UV)
	output.UVMap0 = input.AttrUVMap0;
#endif
#if defined(BUMPMAP_UV)
	output.UVMap1 = input.AttrUVMap1;
#endif
#if defined(SI_UV)
	output.UVMap2 = input.AttrUVMap2;
#endif
#if defined(SPECULAR_UV)
	output.UVMap3 = input.AttrUVMap3;
#endif
	output.PositionInWorld = mul(ObjectToWorld, bonesResult.PositionInObject);
	output.PositionInEye = mul(WorldToEye, output.PositionInWorld);
	output.PositionInProjection = mul(EyeToProjection, output.PositionInEye);
	output.ArrayTextureIndex = MaterialInstanceData.x;
	output.SelfIllumination = SelfIlluminationData;
	return output;
}

#if defined(USE_BONES) && defined(USE_DUALQUATERNION)
BonesResult ApplyBones(VertexIn input, uint instanceBonesOffset)
{
	BonesResult result;
	if (input.AttrBoneWeights.x != 0.0 || input.AttrBoneWeights.y != 0.0 || input.AttrBoneWeights.z != 0.0 || input.AttrBoneWeights.w != 0.0)
	{
		// We use low precision input for our bone weights. Rescale so the sum still is 1.0
		float totalWeight = input.AttrBoneWeights.x + input.AttrBoneWeights.y + input.AttrBoneWeights.z + input.AttrBoneWeights.w;
		float weightMultiplier = 1 / totalWeight;
		input.AttrBoneWeights *= weightMultiplier;

		// For details, read the paper "Geometric Skinning with Dual Skinning" by L. Kavan et al.
		float2x4 dual = (float2x4)0;
		dual  = input.AttrBoneWeights.x * GetBoneDualQuaternion(dual, input.AttrBoneSelectors.x, instanceBonesOffset);
		dual += input.AttrBoneWeights.y * GetBoneDualQuaternion(dual, input.AttrBoneSelectors.y, instanceBonesOffset);
		dual += input.AttrBoneWeights.z * GetBoneDualQuaternion(dual, input.AttrBoneSelectors.z, instanceBonesOffset);
		dual += input.AttrBoneWeights.w * GetBoneDualQuaternion(dual, input.AttrBoneSelectors.w, instanceBonesOffset);
		dual /= length(dual[0]);

		result.PositionInObject.xyz = input.AttrPositionInObject.xyz + 2.0 * cross(dual[0].xyz, cross(dual[0].xyz, input.AttrPositionInObject.xyz) + dual[0].w * input.AttrPositionInObject.xyz);
		result.PositionInObject.xyz += 2.0 * (dual[0].w * dual[1].xyz - dual[1].w * dual[0].xyz + cross(dual[0].xyz, dual[1].xyz));
		result.PositionInObject.w = 1;

		result.Normal = input.AttrNormal + 2.0 * cross(dual[0].xyz, cross(dual[0].xyz, input.AttrNormal) + dual[0].w * input.AttrNormal);
	}
	else
	{
		result.PositionInObject = input.AttrPositionInObject;
		result.Normal = input.AttrNormal;
	}
	return result;
}
#elif defined(USE_BONES)
BonesResult ApplyBones(VertexIn input, uint instanceBonesOffset)
{
	BonesResult result;
	if (input.AttrBoneWeights.x != 0.0 || input.AttrBoneWeights.y != 0.0 || input.AttrBoneWeights.z != 0.0 || input.AttrBoneWeights.w != 0.0)
	{
		// We use low precision input for our bone weights. Rescale so the sum still is 1.0
		float totalWeight = input.AttrBoneWeights.x + input.AttrBoneWeights.y + input.AttrBoneWeights.z + input.AttrBoneWeights.w;
		float weightMultiplier = 1 / totalWeight;
		input.AttrBoneWeights *= weightMultiplier;

		result.PositionInObject = float4(0,0,0,0);
		result.Normal = float3(0,0,0);

		if (input.AttrBoneWeights.x != 0.0)
		{
			float4x4 transform = GetBoneTransform(input.AttrBoneSelectors.x, instanceBonesOffset);
			float3x3 rotation = (float3x3)transform;
			result.PositionInObject += mul(transform, input.AttrPositionInObject) * input.AttrBoneWeights.x;
			result.Normal += mul(rotation, input.AttrNormal) * input.AttrBoneWeights.x;
		}

		if (input.AttrBoneWeights.y != 0.0)
		{
			float4x4 transform = GetBoneTransform(input.AttrBoneSelectors.y, instanceBonesOffset);
			float3x3 rotation = (float3x3)transform;
			result.PositionInObject += mul(transform, input.AttrPositionInObject) * input.AttrBoneWeights.y;
			result.Normal += mul(rotation, input.AttrNormal) * input.AttrBoneWeights.y;
		}

		if (input.AttrBoneWeights.z != 0.0)
		{
			float4x4 transform = GetBoneTransform(input.AttrBoneSelectors.z, instanceBonesOffset);
			float3x3 rotation = (float3x3)transform;
			result.PositionInObject += mul(transform, input.AttrPositionInObject) * input.AttrBoneWeights.z;
			result.Normal += mul(rotation, input.AttrNormal) * input.AttrBoneWeights.z;
		}

		if (input.AttrBoneWeights.w != 0.0)
		{
			float4x4 transform = GetBoneTransform(input.AttrBoneSelectors.w, instanceBonesOffset);
			float3x3 rotation = (float3x3)transform;
			result.PositionInObject += mul(transform, input.AttrPositionInObject) * input.AttrBoneWeights.w;
			result.Normal += mul(rotation, input.AttrNormal) * input.AttrBoneWeights.w;
		}

		result.PositionInObject.w = 1.0; // For numerical stability
	}
	else
	{
		result.PositionInObject = input.AttrPositionInObject;
		result.Normal = input.AttrNormal;
	}
	return result;
}
#else
BonesResult ApplyBones(VertexIn input, uint instanceBonesOffset)
{
	BonesResult result;
	result.PositionInObject = input.AttrPositionInObject;
	result.Normal = input.AttrNormal;
	return result;
}
#endif

#if !defined(USE_DUALQUATERNION)
float4x4 GetBoneTransform(uint boneIndex, uint instanceBonesOffset)
{
	float4 row0 = InstanceVectors.Load(GetTexelPosition(instanceBonesOffset + boneIndex * 3 + 0));
	float4 row1 = InstanceVectors.Load(GetTexelPosition(instanceBonesOffset + boneIndex * 3 + 1));
	float4 row2 = InstanceVectors.Load(GetTexelPosition(instanceBonesOffset + boneIndex * 3 + 2));
	float4 row3 = float4(0,0,0,1);
	return float4x4(row0, row1, row2, row3);
}
#else
float2x4 GetBoneDualQuaternion(float2x4 currentDual, uint boneIndex, uint instanceBonesOffset)
{
	float4 row0 = InstanceVectors.Load(GetTexelPosition(instanceBonesOffset + boneIndex * 2 + 0));
	float4 row1 = InstanceVectors.Load(GetTexelPosition(instanceBonesOffset + boneIndex * 2 + 1));
	float2x4 newDual = float2x4(row0, row1);
	if (dot(currentDual[0], newDual[0]) < 0)
		return -newDual;
	else
		return newDual;
}
#endif

int3 GetTexelPosition(uint index)
{
	int width, height, num_levels;
	InstanceVectors.GetDimensions(0, width, height, num_levels);
	return int3(index % width, index / width, 0);
}

uint GetVectorsOffset(uint instanceId)
{
	return BaseVectorOffset + instanceId * VectorsPerInstance;
}

)shaderend"; } }
