
#pragma once

#include <unordered_map>
#include "model.h"

class Scene_Impl;
class InstancesBuffer;

class ModelCache
{
public:
	ModelCache(Scene_Impl *scene, ModelMaterialCache &texture_cache, ModelShaderCache &shader_cache, InstancesBuffer &instances_buffer);
	std::shared_ptr<Model> get_model(uicore::GraphicContext &gc, const std::string &model_name);

private:
	Scene_Impl *scene;
	std::unordered_map<std::string, std::shared_ptr<Model> > models;
	ModelMaterialCache &texture_cache;
	ModelShaderCache &shader_cache;
	InstancesBuffer &instances_buffer;
};
