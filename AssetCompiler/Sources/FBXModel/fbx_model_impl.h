
#pragma once

#include <fbxsdk.h>
#include <string>

class FBXModelImpl
{
public:
	FBXModelImpl(const std::string &filename);
	~FBXModelImpl();

	void inspect_node(FbxNode *node);
	void inspect_mesh(FbxNode *node);
	void inspect_skins(FbxNode *node, FbxMesh *mesh);
	void inspect_camera(FbxNode *node);
	void inspect_light(FbxNode *node);

	void import_scene(const std::string &filename);
	void scale_scene();
	void triangulate_scene();
	void bake_geometric_transforms(FbxNode *node = nullptr);

	std::string base_path;

	FbxManager *manager;
	FbxIOSettings *iosettings;
	FbxScene *scene;

	std::vector<std::string> material_names;
	std::vector<std::string> bone_names;
	std::vector<std::string> light_names;
	std::vector<std::string> camera_names;
};
