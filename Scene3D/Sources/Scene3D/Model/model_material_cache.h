
#pragma once

#include "Scene3D/SceneCache/resource.h"

class ModelMaterialCache
{
public:
	virtual Resource<uicore::TexturePtr> get_texture(const uicore::GraphicContextPtr &gc, const std::string &material_name, bool linear) = 0;
};
