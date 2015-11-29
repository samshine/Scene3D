
#include "precomp.h"
#include "scene_controller.h"
#include "Model/ModelEditor/model_app_model.h"
#include "Model/ModelEditor/EditorScene/Edit/edit_scene.h"
#include "Model/ModelEditor/EditorScene/Game/game_scene.h"
#include "Views/Scene/scene_view.h"

using namespace uicore;

SceneController::SceneController()
{
	view = std::make_shared<SceneView>();
	set_root_view(view);

	slots.connect(ModelAppModel::instance()->sig_model_data_updated, this, &SceneController::model_data_updated);
	slots.connect(ModelAppModel::instance()->sig_map_model_updated, this, &SceneController::map_model_updated);
	slots.connect(view->sig_update_scene, this, &SceneController::update_scene);

	setup_scene();

	map_model_updated();
	model_data_updated();
}

void SceneController::setup_scene()
{
	if (scene) return;

	scene = Scene::create(SceneView::engine());
	scene->show_skybox_stars(false);
	std::vector<Colorf> gradient;
	gradient.push_back(Colorf(236 * 5 / 10, 240 * 5 / 10, 243 * 5 / 10));
	gradient.push_back(Colorf(236 * 5 / 10, 240 * 5 / 10, 243 * 5 / 10));
	gradient.push_back(Colorf(236 * 5 / 10, 240 * 5 / 10, 243 * 5 / 10));
	gradient.push_back(Colorf(236 * 5 / 10, 240 * 5 / 10, 243 * 5 / 10));
	gradient.push_back(Colorf(236 * 5 / 10, 240 * 5 / 10, 243 * 5 / 10));
	gradient.push_back(Colorf(236 * 5 / 10, 240 * 5 / 10, 243 * 5 / 10));
	gradient.push_back(Colorf(236 * 6 / 10, 240 * 6 / 10, 243 * 6 / 10));
	gradient.push_back(Colorf(236 * 7 / 10, 240 * 7 / 10, 243 * 7 / 10));
	gradient.push_back(Colorf(236 * 8 / 10, 240 * 8 / 10, 243 * 8 / 10));
	for (auto &g : gradient)
	{
		g.r = std::pow(g.r, 2.2f);
		g.g = std::pow(g.g, 2.2f);
		g.b = std::pow(g.b, 2.2f);
	}
	scene->set_skybox_gradient(gradient);

	view->viewport()->set_camera(SceneCamera::create(scene));
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

void SceneController::update_scene(const SceneViewportPtr &scene_viewport, const GraphicContextPtr &gc, const DisplayWindowPtr &ic, const uicore::Vec2i &mouse_delta)
{
	bool has_focus = view.get() == view->focus_view();
	ModelAppModel::instance()->editor_scene->update(scene, scene_viewport, gc, ic, has_focus, mouse_delta);
}
