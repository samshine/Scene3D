
#pragma once

#include "model_instance.h"

class ModelData;
class ModelMesh;
class SceneEngineImpl;

class Model
{
public:
	Model(SceneEngineImpl *engine, std::shared_ptr<ModelData> model_data);

	void create_mesh(const uicore::GraphicContextPtr &gc);

	SceneEngineImpl *engine;
	std::shared_ptr<ModelData> model_data;
	std::shared_ptr<ModelMesh> mesh;
	int model_index;
};
