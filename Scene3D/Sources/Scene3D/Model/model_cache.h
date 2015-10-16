
#pragma once

#include <unordered_map>
#include "model.h"

class SceneImpl;
class InstancesBuffer;

class ModelCache
{
public:
	ModelCache(SceneImpl *scene, ModelMaterialCache &texture_cache, ModelShaderCache &shader_cache, InstancesBuffer &instances_buffer);
	std::shared_ptr<Model> get_model(const std::string &model_name);

private:
	SceneImpl *scene;
	std::unordered_map<std::string, std::shared_ptr<Model> > models;
	ModelMaterialCache &texture_cache;
	ModelShaderCache &shader_cache;
	InstancesBuffer &instances_buffer;
};
