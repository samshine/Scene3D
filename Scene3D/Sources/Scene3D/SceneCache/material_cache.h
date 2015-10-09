
#pragma once

#include "Scene3D/Model/model_material_cache.h"
#include <unordered_map>

class Scene_Impl;

class MaterialCache : public ModelMaterialCache
{
public:
	MaterialCache(Scene_Impl *scene);
	Resource<uicore::TexturePtr> get_texture(const uicore::GraphicContextPtr &gc, const std::string &material_name, bool linear);

	void update(const uicore::GraphicContextPtr &gc, float time_elapsed);

private:
	Scene_Impl *scene;
};
