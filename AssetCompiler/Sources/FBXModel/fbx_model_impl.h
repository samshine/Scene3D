
#pragma once

#include "AssetCompiler/FBXModel/fbx_model.h"

#include <fbxsdk.h>
#include <string>

class FBXModelImpl : public FBXModel
{
public:
	FBXModelImpl(const std::string &filename);
	~FBXModelImpl();

	const std::vector<std::string> &material_names() const override;
	const std::vector<std::string> &bone_names() const override;
	const std::vector<std::string> &light_names() const override;
	const std::vector<std::string> &camera_names() const override;

	std::shared_ptr<ModelData> convert(const ModelDesc &desc);

private:
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

	std::vector<std::string> _material_names;
	std::vector<std::string> _bone_names;
	std::vector<std::string> _light_names;
	std::vector<std::string> _camera_names;

	friend class FBXModelLoader;
};
