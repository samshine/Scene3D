
#pragma once

#include "Scene3D/SceneCache/resource.h"

class ModelMaterialCache
{
public:
	virtual Resource<uicore::Texture> get_texture(uicore::GraphicContext &gc, const std::string &material_name, bool linear) = 0;
};
