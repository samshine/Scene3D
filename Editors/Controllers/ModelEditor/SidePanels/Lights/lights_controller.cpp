
#include "precomp.h"
#include "lights_controller.h"
#include "Views/ModelEditor/lights_sidepanel.h"
#include "Model/ModelEditor/model_app_model.h"

using namespace uicore;

LightsController::LightsController()
{
	panel = view->content_view()->add_child<LightsSidePanelView>();

	slots.connect(panel->lights_list->sig_selection_changed(), this, &LightsController::lights_list_selection_changed);
	slots.connect(panel->lights_list->sig_selection_clicked(), this, &LightsController::lights_list_selection_clicked);

	slots.connect(ModelAppModel::instance()->sig_load_finished, [this]() { update_lights(); });

	update_lights();
}

void LightsController::update_lights()
{
	panel->lights_list->clear();

	if (ModelAppModel::instance()->fbx)
	{
		for (const auto &light_name : ModelAppModel::instance()->fbx->light_names())
		{
			panel->lights_list->add_item(light_name);
		}
	}
	/*
	for (const auto &light : ModelAppModel::instance()->desc.lights)
	{
		auto &item = items[light.mesh_light];
		if (!item)
			item = lights_list->add_item(light.mesh_light);

		item->set_bold(true);

		if (first)
		{
			item->set_selected(true, false);
			first = false;
		}
	}
	*/

	if (panel->lights_list->selected_item() == -1)
		panel->light->set_hidden(true);
}

int LightsController::get_select_item_index()
{
	/*auto selection = lights_list->selection();
	if (selection)
	{
		std::string name = selection->text();

		const auto &lights = ModelAppModel::instance()->desc.lights;
		for (size_t i = 0; i < lights.size(); i++)
		{
			if (lights[i].mesh_light == name)
				return (int)i;
		}
	}*/
	return -1;
}

void LightsController::update_light_fields()
{
	int index = get_select_item_index();

	if (index >= 0)
	{
		panel->light->set_hidden(false);

		//const auto &light = ModelAppModel::instance()->desc.lights[index];
	}
	else
	{
		panel->light->set_hidden(true);
	}
}

void LightsController::lights_list_selection_changed()
{
	update_light_fields();
}

void LightsController::lights_list_selection_clicked()
{
	auto selection = panel->lights_list->selected_item();
	if (selection != -1)
	{
		/*
		if (selection->index >= ModelAppModel::instance()->desc.lights.size())
		{
			ModelAppModel::instance()->desc.lights.resize(selection->index + 1);
			lights_list->add_item("");
		}

		auto &light = ModelAppModel::instance()->desc.lights[selection->index];
		light.name = selection->text();

		update_light_fields();
		*/
	}
}
