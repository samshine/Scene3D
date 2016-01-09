
#include "precomp.h"
#include "model.h"
#include "model_mesh.h"
#include "Scene3D/SceneEngine/scene_engine_impl.h"

using namespace uicore;

Model::Model(SceneEngineImpl *engine, std::shared_ptr<ModelData> model_data) : engine(engine), model_data(model_data)
{
}

void Model::create_mesh(const uicore::GraphicContextPtr &gc)
{
	if (!mesh)
		mesh = std::make_shared<ModelMesh>(engine, gc, model_data);
}
