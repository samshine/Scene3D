
#pragma once

#include <fbxsdk.h>
#include <memory>
#include <string>
#include <vector>

namespace clan { class ModelData; }

class Animation;
class AttachmentPoint;
class ParticleEmitter;

class FBXModel
{
public:
	FBXModel(const std::string &filename);
	~FBXModel();

	std::shared_ptr<clan::ModelData> convert(const std::vector<Animation> &animations, const std::vector<AttachmentPoint> &attachment_points, const std::vector<ParticleEmitter> &emitters);

private:
	void import_scene(const std::string &filename);
	void triangulate_scene();
	void bake_geometric_transforms(FbxNode *node = nullptr);

	FBXModel(const FBXModel &) = delete;
	FBXModel &operator=(const FBXModel &) = delete;

	std::string base_path;

	FbxManager *manager;
	FbxIOSettings *iosettings;
	FbxScene *scene;

	friend class FBXModelLoader;
};
