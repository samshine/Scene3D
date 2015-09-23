
#pragma once

#include <fbxsdk.h>

namespace uicore
{
	class SkinnedBone
	{
	public:
		SkinnedBone() : bone_node(nullptr) { }

		FbxAMatrix bind_bone_to_world;
		FbxNode *bone_node;
	};
}
