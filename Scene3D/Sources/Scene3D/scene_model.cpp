
#include "precomp.h"
#include "Scene3D/scene_model.h"
#include "Scene3D/scene.h"
#include "Scene3D/Model/model_cache.h"
#include "scene_impl.h"
#include "scene_model_impl.h"

using namespace uicore;

SceneModel::SceneModel()
{
}

SceneModel::SceneModel(Scene &scene, const std::string &model_name)
: impl(std::make_shared<SceneModel_Impl>())
{
	impl->scene = scene.impl.get();
	impl->model = impl->scene->model_cache->get_model(model_name);
}

SceneModel::SceneModel(Scene &scene, std::shared_ptr<ModelData> model_data)
: impl(std::make_shared<SceneModel_Impl>())
{
	impl->scene = scene.impl.get();
	impl->model = std::shared_ptr<Model>(new Model(impl->scene->get_cache()->get_gc(), *impl->scene->material_cache, *impl->scene->model_shader_cache, model_data, impl->scene->instances_buffer.new_offset_index()));
}

bool SceneModel::is_null() const
{
	return !impl;
}
