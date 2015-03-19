
#include "precomp.h"
#include "map_scene_controller.h"
#include "Model/MapEditor/map_app_model.h"
#include "Views/Scene/scene_view.h"

using namespace clan;

MapSceneController::MapSceneController()
{
	view = std::make_shared<SceneView>();

	slots.connect(MapAppModel::instance()->sig_model_data_updated, this, &MapSceneController::model_data_updated);
	slots.connect(MapAppModel::instance()->sig_map_model_updated, this, &MapSceneController::map_model_updated);
	slots.connect(scene_view()->sig_update_scene, this, &MapSceneController::update_scene);

	map_model_updated();
	model_data_updated();
}

std::shared_ptr<SceneView> MapSceneController::scene_view()
{
	return std::static_pointer_cast<SceneView>(view);
}

void MapSceneController::model_data_updated()
{
	/*
	MapAppModel::instance()->editor_scene->set_model_data(MapAppModel::instance()->model_data);

	std::vector<SceneModelAttachment> attachments;
	for (const auto &fbx_attach : MapAppModel::instance()->desc.attachment_points)
	{
		attachments.push_back(SceneModelAttachment(fbx_attach.name, fbx_attach.test_model, fbx_attach.test_scale));
	}
	MapAppModel::instance()->editor_scene->set_attachments(attachments);
	*/
}

void MapSceneController::map_model_updated()
{
	//MapAppModel::instance()->editor_scene->set_map_model(MapAppModel::instance()->map_model);
}

void MapSceneController::update_scene(Scene &scene, GraphicContext &gc, InputContext &ic, const clan::Vec2i &mouse_delta)
{
	//bool has_focus = view.get() == view->focus_view();
	//MapAppModel::instance()->editor_scene->update(scene, gc, ic, has_focus, mouse_delta);
}
