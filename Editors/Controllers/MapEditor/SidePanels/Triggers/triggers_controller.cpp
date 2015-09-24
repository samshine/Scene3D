
#include "precomp.h"
#include "triggers_controller.h"
#include "Views/Rollout/rollout_view.h"
#include "Views/Rollout/rollout_list.h"
#include "Views/Rollout/rollout_text_field_property.h"
#include "Model/MapEditor/map_app_model.h"

using namespace uicore;

TriggersController::TriggersController()
{
	triggers = std::make_shared<RolloutView>("TRIGGERS");
	trigger = std::make_shared<RolloutView>("TRIGGER");

	view->content_view()->add_subview(triggers);
	view->content_view()->add_subview(trigger);

	triggers_list = std::make_shared<RolloutList>();
	triggers_list->set_allow_edit(false);

	triggers->content->add_subview(triggers_list);

	slots.connect(triggers_list->sig_selection_changed(), this, &TriggersController::triggers_list_selection_changed);
	slots.connect(triggers_list->sig_selection_clicked(), this, &TriggersController::triggers_list_selection_clicked);

	slots.connect(MapAppModel::instance()->sig_load_finished, [this]() { update_triggers(); });

	update_triggers();
}

void TriggersController::update_triggers()
{
	triggers_list->clear();
	bool first = true;

	std::map<std::string, std::shared_ptr<RolloutListItemView>> items;

	/*
	if (MapAppModel::instance()->fbx)
	{
		for (const auto &trigger_name : MapAppModel::instance()->fbx->trigger_names())
		{
			auto item = triggers_list->add_item(trigger_name);
			if (first)
			{
				item->set_selected(true, false);
				first = false;
			}
			items[trigger_name] = item;
		}
	}
	*/
	/*
	for (const auto &trigger : MapAppModel::instance()->desc.triggers)
	{
		auto &item = items[trigger.mesh_trigger];
		if (!item)
			item = triggers_list->add_item(trigger.mesh_trigger);

		item->set_bold(true);

		if (first)
		{
			item->set_selected(true, false);
			first = false;
		}
	}
	*/

	if (!triggers_list->selection())
		trigger->set_hidden(true);
}

int TriggersController::get_select_item_index()
{
	/*auto selection = triggers_list->selection();
	if (selection)
	{
		std::string name = selection->text();

		const auto &triggers = MapAppModel::instance()->desc.triggers;
		for (size_t i = 0; i < triggers.size(); i++)
		{
			if (triggers[i].mesh_trigger == name)
				return (int)i;
		}
	}*/
	return -1;
}

void TriggersController::update_trigger_fields()
{
	int index = get_select_item_index();

	if (index >= 0)
	{
		trigger->set_hidden(false);

		//const auto &trigger = MapAppModel::instance()->desc.triggers[index];
	}
	else
	{
		trigger->set_hidden(true);
	}
}

void TriggersController::triggers_list_selection_changed()
{
	update_trigger_fields();
}

void TriggersController::triggers_list_selection_clicked()
{
	auto selection = triggers_list->selection();
	if (selection)
	{
		/*
		if (selection->index >= MapAppModel::instance()->desc.triggers.size())
		{
			MapAppModel::instance()->desc.triggers.resize(selection->index + 1);
			triggers_list->add_item("");
		}

		auto &trigger = MapAppModel::instance()->desc.triggers[selection->index];
		trigger.name = selection->text();

		update_trigger_fields();
		*/
	}
}
