
#include "precomp.h"
#include "map_light_probes_controller.h"
#include "Views/Rollout/rollout_view.h"
#include "Views/Rollout/rollout_list.h"
#include "Views/Rollout/rollout_text_field_property.h"
#include "Views/Rollout/rollout_position_property.h"
#include "Model/MapEditor/map_app_model.h"

using namespace uicore;

MapLightProbesController::MapLightProbesController()
{
	light_probes = std::make_shared<RolloutView>("LIGHT PROBES");
	light_probe = std::make_shared<RolloutView>("LIGHT PROBE");
	position = std::make_shared<RolloutPositionProperty>("POSITION");

	content_view()->add_subview(light_probes);
	content_view()->add_subview(light_probe);

	light_probes_list = std::make_shared<RolloutList>();
	light_probes_list->set_allow_edit(false);

	light_probes->content->add_subview(light_probes_list);

	light_probe->content->add_subview(position);

	slots.connect(light_probes_list->sig_selection_changed(), this, &MapLightProbesController::light_probes_list_selection_changed);
	slots.connect(light_probes_list->sig_selection_clicked(), this, &MapLightProbesController::light_probes_list_selection_clicked);

	slots.connect(MapAppModel::instance()->sig_load_finished, [this]() { update_light_probes(); });

	update_light_probes();
}

void MapLightProbesController::update_light_probes()
{
	light_probes_list->clear();
	bool first = true;

	std::map<std::string, std::shared_ptr<RolloutListItemView>> items;

	/*
	if (MapAppModel::instance()->fbx)
	{
		for (const auto &light_probe_name : MapAppModel::instance()->fbx->light_probe_names())
		{
			auto item = light_probes_list->add_item(light_probe_name);
			if (first)
			{
				item->set_selected(true, false);
				first = false;
			}
			items[light_probe_name] = item;
		}
	}
	*/
	/*
	for (const auto &light_probe : MapAppModel::instance()->desc.light_probes)
	{
		auto &item = items[light_probe.mesh_light_probe];
		if (!item)
			item = light_probes_list->add_item(light_probe.mesh_light_probe);

		item->set_bold(true);

		if (first)
		{
			item->set_selected(true, false);
			first = false;
		}
	}
	*/

	//if (!light_probes_list->selection())
	//	light_probe->set_hidden(true);
}

int MapLightProbesController::get_select_item_index()
{
	/*auto selection = light_probes_list->selection();
	if (selection)
	{
		std::string name = selection->text();

		const auto &light_probes = MapAppModel::instance()->desc.light_probes;
		for (size_t i = 0; i < light_probes.size(); i++)
		{
			if (light_probes[i].mesh_light_probe == name)
				return (int)i;
		}
	}*/
	return -1;
}

void MapLightProbesController::update_light_probe_fields()
{
	int index = get_select_item_index();

	if (index >= 0)
	{
		light_probe->set_hidden(false);

		//const auto &light_probe = MapAppModel::instance()->desc.light_probes[index];
	}
	else
	{
		light_probe->set_hidden(true);
	}
}

void MapLightProbesController::light_probes_list_selection_changed()
{
	update_light_probe_fields();
}

void MapLightProbesController::light_probes_list_selection_clicked()
{
	auto selection = light_probes_list->selection();
	if (selection)
	{
		/*
		if (selection->index >= MapAppModel::instance()->desc.light_probes.size())
		{
			MapAppModel::instance()->desc.light_probes.resize(selection->index + 1);
			light_probes_list->add_item("");
		}

		auto &light_probe = MapAppModel::instance()->desc.light_probes[selection->index];
		light_probe.name = selection->text();

		update_light_probe_fields();
		*/
	}
}
