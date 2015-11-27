
#include "precomp.h"
#include "Scene3D/scene_model.h"
#include "Scene3D/scene.h"
#include "Scene3D/SceneEngine/Model/model.h"
#include "scene_impl.h"
#include "scene_model_impl.h"

using namespace uicore;

std::shared_ptr<SceneModel> SceneModel::create(const ScenePtr &scene, const std::string &model_name)
{
	auto scene_impl = static_cast<SceneImpl*>(scene.get());
	return std::make_shared<SceneModelImpl>(scene_impl, scene_impl->engine()->get_model(model_name));
}

std::shared_ptr<SceneModel> SceneModel::create(const ScenePtr &scene, std::shared_ptr<ModelData> model_data)
{
	auto scene_impl = static_cast<SceneImpl*>(scene.get());
	auto model = std::make_shared<Model>(scene_impl->engine(), model_data);
	return std::make_shared<SceneModelImpl>(scene_impl, model);
}
