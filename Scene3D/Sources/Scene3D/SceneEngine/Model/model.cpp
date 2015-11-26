
#include "precomp.h"
#include "model.h"
#include "model_shader_cache.h"
#include "Scene3D/ModelData/model_data.h"
#include "dual_quaternion.h"
#include "model_lod.h"
#include "Scene3D/SceneEngine/instances_buffer.h"
#include "Scene3D/SceneEngine/scene_engine_impl.h"

using namespace uicore;

Model::Model(SceneEngineImpl *engine, std::shared_ptr<ModelData> model_data, int model_index) : engine(engine), model_data(model_data), model_index(model_index)
{
}

void Model::create_mesh(const uicore::GraphicContextPtr &gc)
{
	if (!mesh)
		mesh = std::make_shared<ModelLOD>(engine, gc, model_index, model_data);
}
