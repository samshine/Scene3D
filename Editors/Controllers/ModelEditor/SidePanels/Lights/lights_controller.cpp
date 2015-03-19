
#include "precomp.h"
#include "lights_controller.h"
#include "Views/Rollout/rollout_view.h"
#include "Views/Rollout/rollout_list.h"
#include "Views/Rollout/rollout_text_field_property.h"
#include "Model/ModelEditor/model_app_model.h"

using namespace clan;

LightsController::LightsController()
{
	view->style()->set("flex-direction: column");

	lights = std::make_shared<RolloutView>("LIGHTS");
	light = std::make_shared<RolloutView>("LIGHT");

	view->add_subview(lights);
	view->add_subview(light);

	lights_list = std::make_shared<RolloutList>();
	lights_list->set_allow_edit(false);

	lights->content->add_subview(lights_list);

	slots.connect(lights_list->sig_selection_changed(), this, &LightsController::lights_list_selection_changed);
	slots.connect(lights_list->sig_selection_clicked(), this, &LightsController::lights_list_selection_clicked);

	slots.connect(ModelAppModel::instance()->sig_load_finished, [this]() { update_lights(); });

	update_lights();
}

void LightsController::update_lights()
{
	lights_list->clear();
	bool first = true;

	std::map<std::string, std::shared_ptr<RolloutListItemView>> items;

	if (ModelAppModel::instance()->fbx)
	{
		for (const auto &light_name : ModelAppModel::instance()->fbx->light_names())
		{
			auto item = lights_list->add_item(light_name);
			if (first)
			{
				item->set_selected(true, false);
				first = false;
			}
			items[light_name] = item;
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

	if (!lights_list->selection())
		light->set_hidden(true);
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
		light->set_hidden(false);

		//const auto &light = ModelAppModel::instance()->desc.lights[index];
	}
	else
	{
		light->set_hidden(true);
	}
}

void LightsController::lights_list_selection_changed()
{
	update_light_fields();
}

void LightsController::lights_list_selection_clicked()
{
	auto selection = lights_list->selection();
	if (selection)
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
