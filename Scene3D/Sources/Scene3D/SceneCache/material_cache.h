
#pragma once

#include "Scene3D/Model/model_material_cache.h"
#include <unordered_map>

class Scene_Impl;

class MaterialCache : public ModelMaterialCache
{
public:
	MaterialCache(Scene_Impl *scene);
	Resource<uicore::Texture> get_texture(uicore::GraphicContext &gc, const std::string &material_name, bool linear);

	void update(uicore::GraphicContext &gc, float time_elapsed);

private:
	Scene_Impl *scene;
};
