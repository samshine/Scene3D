
#include "precomp.h"
#include "map_scene_controller.h"
#include "Model/MapEditor/map_app_model.h"
#include "Views/Scene/scene_view.h"

using namespace uicore;

MapSceneController::MapSceneController()
{
	set_root_view(view);

	slots.connect(MapAppModel::instance()->sig_map_model_data_updated, this, &MapSceneController::map_model_data_updated);
	slots.connect(view->sig_update_scene, this, &MapSceneController::update_scene);
}

void MapSceneController::map_model_data_updated()
{
	map_object = SceneObject();
}

void MapSceneController::update_scene(Scene &scene, const uicore::GraphicContextPtr &gc, const uicore::DisplayWindowPtr &ic, const uicore::Vec2i &)
{
	if (map_object.is_null())
	{
		if (!MapAppModel::instance()->map_model_data)
			return;

		SceneModel model(scene, MapAppModel::instance()->map_model_data);
		map_object = SceneObject(scene, model);
	}
}
