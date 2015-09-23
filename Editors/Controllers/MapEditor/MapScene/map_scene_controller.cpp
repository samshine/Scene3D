
#include "precomp.h"
#include "map_scene_controller.h"
#include "Model/MapEditor/map_app_model.h"
#include "Views/Scene/scene_view.h"

using namespace uicore;

MapSceneController::MapSceneController()
{
	view = std::make_shared<SceneView>();

	slots.connect(MapAppModel::instance()->sig_map_model_data_updated, this, &MapSceneController::map_model_data_updated);
	slots.connect(scene_view()->sig_update_scene, this, &MapSceneController::update_scene);
}

std::shared_ptr<SceneView> MapSceneController::scene_view()
{
	return std::static_pointer_cast<SceneView>(view);
}

void MapSceneController::map_model_data_updated()
{
	map_object = uicore::SceneObject();
}

void MapSceneController::update_scene(uicore::Scene &scene, uicore::GraphicContext &gc, uicore::DisplayWindow &ic, const uicore::Vec2i &)
{
	if (map_object.is_null())
	{
		if (!MapAppModel::instance()->map_model_data)
			return;

		SceneModel model(gc, scene, MapAppModel::instance()->map_model_data);
		map_object = SceneObject(scene, model);
	}
}
