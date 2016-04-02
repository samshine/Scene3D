
#include "precomp.h"
#include "cameras_controller.h"
#include "Views/ModelEditor/cameras_sidepanel.h"
#include "Model/ModelEditor/model_app_model.h"

using namespace uicore;

CamerasController::CamerasController()
{
	panel = view->content_view()->add_child<CamerasSidePanelView>();

	slots.connect(panel->cameras_list->sig_selection_changed(), this, &CamerasController::cameras_list_selection_changed);
	slots.connect(panel->cameras_list->sig_selection_clicked(), this, &CamerasController::cameras_list_selection_clicked);

	slots.connect(ModelAppModel::instance()->sig_load_finished, [this]() { update_cameras(); });

	update_cameras();
}

void CamerasController::update_cameras()
{
	panel->cameras_list->clear();
	bool first = true;

	if (ModelAppModel::instance()->fbx)
	{
		for (const auto &camera_name : ModelAppModel::instance()->fbx->camera_names())
		{
			panel->cameras_list->add_item(camera_name);
		}
	}
	/*
	for (const auto &camera : ModelAppModel::instance()->desc.cameras)
	{
		auto &item = items[camera.mesh_camera];
		if (!item)
			item = cameras_list->add_item(camera.mesh_camera);

		item->set_bold(true);

		if (first)
		{
			item->set_selected(true, false);
			first = false;
		}
	}
	*/

	if (panel->cameras_list->selected_item() == -1)
		panel->camera->set_hidden(true);
}

int CamerasController::get_select_item_index()
{
	/*auto selection = cameras_list->selection();
	if (selection)
	{
		std::string name = selection->text();

		const auto &cameras = ModelAppModel::instance()->desc.cameras;
		for (size_t i = 0; i < cameras.size(); i++)
		{
			if (cameras[i].mesh_camera == name)
				return (int)i;
		}
	}*/
	return -1;
}

void CamerasController::update_camera_fields()
{
	int index = get_select_item_index();

	if (index >= 0)
	{
		panel->camera->set_hidden(false);

		//const auto &camera = ModelAppModel::instance()->desc.cameras[index];
	}
	else
	{
		panel->camera->set_hidden(true);
	}
}

void CamerasController::cameras_list_selection_changed()
{
	update_camera_fields();
}

void CamerasController::cameras_list_selection_clicked()
{
	auto selection = panel->cameras_list->selected_item();
	if (selection != -1)
	{
		/*
		if (selection->index >= ModelAppModel::instance()->desc.cameras.size())
		{
			ModelAppModel::instance()->desc.cameras.resize(selection->index + 1);
			cameras_list->add_item("");
		}

		auto &camera = ModelAppModel::instance()->desc.cameras[selection->index];
		camera.name = selection->text();

		update_camera_fields();
		*/
	}
}
