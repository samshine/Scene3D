
#pragma once

#include <fbxsdk.h>
#include <string>

namespace clan
{
	class FBXModelImpl
	{
	public:
		FBXModelImpl(const std::string &filename);
		~FBXModelImpl();

		void import_scene(const std::string &filename);
		void triangulate_scene();
		void bake_geometric_transforms(FbxNode *node = nullptr);

		std::string base_path;

		FbxManager *manager;
		FbxIOSettings *iosettings;
		FbxScene *scene;
	};
}
