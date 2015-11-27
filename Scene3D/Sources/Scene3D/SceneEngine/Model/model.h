
#pragma once

#include "Scene3D/ModelData/model_data.h"
#include "model_instance.h"

class ModelDataLight;
class ModelLightInstance;
class ModelMeshVisitor;
class ModelLOD;
class ModelInstancesBuffer;
class SceneEngineImpl;

class Model
{
public:
	Model(SceneEngineImpl *engine, std::shared_ptr<ModelData> model_data);

	void create_mesh(const uicore::GraphicContextPtr &gc);

	SceneEngineImpl *engine;
	std::shared_ptr<ModelData> model_data;
	std::shared_ptr<ModelLOD> mesh;
	int model_index;
};
