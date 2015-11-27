
#include "precomp.h"
#include "model.h"
#include "model_shader_cache.h"
#include "Scene3D/ModelData/model_data.h"
#include "dual_quaternion.h"
#include "model_mesh.h"
#include "model_instances_buffer.h"
#include "Scene3D/SceneEngine/scene_engine_impl.h"

using namespace uicore;

Model::Model(SceneEngineImpl *engine, std::shared_ptr<ModelData> model_data) : engine(engine), model_data(model_data)
{
	model_index = engine->render.model_render.new_model_index();
}

void Model::create_mesh(const uicore::GraphicContextPtr &gc)
{
	if (!mesh)
		mesh = std::make_shared<ModelMesh>(engine, gc, model_index, model_data);
}
