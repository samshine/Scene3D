
#include "precomp.h"
#include "Scene3D/scene_model.h"
#include "Scene3D/scene.h"
#include "Scene3D/Model/model_cache.h"
#include "scene_impl.h"
#include "scene_model_impl.h"

using namespace uicore;

std::shared_ptr<SceneModel> SceneModel::create(const ScenePtr &scene, const std::string &model_name)
{
	auto scene_impl = static_cast<Scene_Impl*>(scene.get());
	return std::make_shared<SceneModel_Impl>(scene_impl, scene_impl->model_cache->get_model(model_name));
}

std::shared_ptr<SceneModel> SceneModel::create(const ScenePtr &scene, std::shared_ptr<ModelData> model_data)
{
	auto scene_impl = static_cast<Scene_Impl*>(scene.get());
	auto model = std::make_shared<Model>(scene_impl->get_cache()->get_gc(), *scene_impl->material_cache, *scene_impl->model_shader_cache, model_data, scene_impl->instances_buffer.new_offset_index());
	return std::make_shared<SceneModel_Impl>(scene_impl, model);
}
