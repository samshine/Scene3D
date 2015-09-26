
#pragma once

#include <fbxsdk.h>

class SkinnedBone
{
public:
	FbxAMatrix bind_bone_to_world;
	FbxNode *bone_node = nullptr;
};
