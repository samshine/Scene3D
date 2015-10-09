
#pragma once

#include "Scene3D/ModelData/model_data.h"

class ModelLOD;

class ModelMeshVisitor
{
public:
	virtual void render(const uicore::GraphicContextPtr &gc, ModelLOD *model_lod, int num_instances) = 0;
};
