
#include "precomp.h"
#include "map_emitters_controller.h"
#include "Views/Rollout/rollout_view.h"
#include "Views/Rollout/rollout_list.h"
#include "Views/Rollout/rollout_text_field_property.h"
#include "Model/MapEditor/map_app_model.h"

using namespace uicore;

MapEmittersController::MapEmittersController()
{
	emitters = std::make_shared<RolloutView>("EMITTERS");
	emitter = std::make_shared<RolloutView>("EMITTER");

	content_view()->add_subview(emitters);
	content_view()->add_subview(emitter);

	emitters_list = std::make_shared<RolloutList>();
	emitters_list->set_allow_edit(false);

	emitters->content->add_subview(emitters_list);

	slots.connect(emitters_list->sig_selection_changed(), this, &MapEmittersController::emitters_list_selection_changed);
	slots.connect(emitters_list->sig_selection_clicked(), this, &MapEmittersController::emitters_list_selection_clicked);

	slots.connect(MapAppModel::instance()->sig_load_finished, [this]() { update_emitters(); });

	update_emitters();
}

void MapEmittersController::update_emitters()
{
	emitters_list->clear();
	bool first = true;

	std::map<std::string, std::shared_ptr<RolloutListItemView>> items;

	/*
	if (MapAppModel::instance()->fbx)
	{
		for (const auto &emitter_name : MapAppModel::instance()->fbx->emitter_names())
		{
			auto item = emitters_list->add_item(emitter_name);
			if (first)
			{
				item->set_selected(true, false);
				first = false;
			}
			items[emitter_name] = item;
		}
	}
	*/
	/*
	for (const auto &emitter : MapAppModel::instance()->desc.emitters)
	{
		auto &item = items[emitter.mesh_emitter];
		if (!item)
			item = emitters_list->add_item(emitter.mesh_emitter);

		item->set_bold(true);

		if (first)
		{
			item->set_selected(true, false);
			first = false;
		}
	}
	*/

	if (!emitters_list->selection())
		emitter->set_hidden(true);
}

int MapEmittersController::get_select_item_index()
{
	/*auto selection = emitters_list->selection();
	if (selection)
	{
		std::string name = selection->text();

		const auto &emitters = MapAppModel::instance()->desc.emitters;
		for (size_t i = 0; i < emitters.size(); i++)
		{
			if (emitters[i].mesh_emitter == name)
				return (int)i;
		}
	}*/
	return -1;
}

void MapEmittersController::update_emitter_fields()
{
	int index = get_select_item_index();

	if (index >= 0)
	{
		emitter->set_hidden(false);

		//const auto &emitter = MapAppModel::instance()->desc.emitters[index];
	}
	else
	{
		emitter->set_hidden(true);
	}
}

void MapEmittersController::emitters_list_selection_changed()
{
	update_emitter_fields();
}

void MapEmittersController::emitters_list_selection_clicked()
{
	auto selection = emitters_list->selection();
	if (selection)
	{
		/*
		if (selection->index >= MapAppModel::instance()->desc.emitters.size())
		{
			MapAppModel::instance()->desc.emitters.resize(selection->index + 1);
			emitters_list->add_item("");
		}

		auto &emitter = MapAppModel::instance()->desc.emitters[selection->index];
		emitter.name = selection->text();

		update_emitter_fields();
		*/
	}
}
