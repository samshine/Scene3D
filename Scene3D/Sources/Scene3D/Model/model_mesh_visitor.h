
#pragma once

#include "Scene3D/ModelData/model_data.h"

class ModelLOD;

class ModelMeshVisitor
{
public:
	virtual void render(uicore::GraphicContext &gc, ModelLOD *model_lod, int num_instances) = 0;
};
