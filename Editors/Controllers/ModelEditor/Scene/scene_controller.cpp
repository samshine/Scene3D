
#include "precomp.h"
#include "scene_controller.h"
#include "Model/ModelEditor/model_app_model.h"
#include "Model/ModelEditor/EditorScene/Edit/edit_scene.h"
#include "Model/ModelEditor/EditorScene/Game/game_scene.h"
#include "Views/Scene/scene_view.h"

using namespace clan;

SceneController::SceneController()
{
	view = std::make_shared<SceneView>();

	slots.connect(ModelAppModel::instance()->sig_model_data_updated, this, &SceneController::model_data_updated);
	slots.connect(ModelAppModel::instance()->sig_map_model_updated, this, &SceneController::map_model_updated);
	slots.connect(scene_view()->sig_update_scene, this, &SceneController::update_scene);

	map_model_updated();
	model_data_updated();
}

std::shared_ptr<SceneView> SceneController::scene_view()
{
	return std::static_pointer_cast<SceneView>(view);
}

void SceneController::model_data_updated()
{
	ModelAppModel::instance()->editor_scene->set_model_data(ModelAppModel::instance()->model_data);

	std::vector<SceneModelAttachment> attachments;
	for (const auto &fbx_attach : ModelAppModel::instance()->desc.attachment_points)
	{
		attachments.push_back(SceneModelAttachment(fbx_attach.name, fbx_attach.test_model, fbx_attach.test_scale));
	}
	ModelAppModel::instance()->editor_scene->set_attachments(attachments);
}

void SceneController::map_model_updated()
{
	ModelAppModel::instance()->editor_scene->set_map_model(ModelAppModel::instance()->map_model);
}

void SceneController::update_scene(Scene &scene, GraphicContext &gc, InputContext &ic, const clan::Vec2i &mouse_delta)
{
	bool has_focus = view.get() == view->focus_view();
	ModelAppModel::instance()->editor_scene->update(scene, gc, ic, has_focus, mouse_delta);
}
