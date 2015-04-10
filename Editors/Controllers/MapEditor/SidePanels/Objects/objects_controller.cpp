
#include "precomp.h"
#include "objects_controller.h"
#include "Views/Rollout/rollout_view.h"
#include "Views/Rollout/rollout_list.h"
#include "Views/Rollout/rollout_text_field_property.h"
#include "Views/Rollout/rollout_position_property.h"
#include "Views/Rollout/rollout_browse_field_property.h"
#include "Model/MapEditor/map_app_model.h"

using namespace clan;

ObjectsController::ObjectsController()
{
	objects = std::make_shared<RolloutView>("OBJECTS");
	object = std::make_shared<RolloutView>("OBJECT");
	id = std::make_shared<RolloutTextFieldProperty>("ID");
	position = std::make_shared<RolloutPositionProperty>("POSITION");
	dir = std::make_shared<RolloutTextFieldProperty>("DIR");
	up = std::make_shared<RolloutTextFieldProperty>("UP");
	tilt = std::make_shared<RolloutTextFieldProperty>("TILT");
	scale = std::make_shared<RolloutTextFieldProperty>("SCALE");
	model_desc_filename = std::make_shared<RolloutBrowseFieldProperty>("MODEL");

	content_view()->add_subview(objects);
	content_view()->add_subview(object);

	objects_list = std::make_shared<RolloutList>();
	objects_list->set_allow_edit(false);

	objects->content->add_subview(objects_list);

	object->content->add_subview(id);
	object->content->add_subview(position);
	object->content->add_subview(dir);
	object->content->add_subview(up);
	object->content->add_subview(tilt);
	object->content->add_subview(scale);
	object->content->add_subview(model_desc_filename);

	slots.connect(objects_list->sig_selection_changed(), this, &ObjectsController::objects_list_selection_changed);
	slots.connect(objects_list->sig_selection_clicked(), this, &ObjectsController::objects_list_selection_clicked);

	slots.connect(MapAppModel::instance()->sig_load_finished, [this]() { update_objects(); });

	update_objects();
}

void ObjectsController::update_objects()
{
	objects_list->clear();
	bool first = true;

	std::map<std::string, std::shared_ptr<RolloutListItemView>> items;

	/*
	if (MapAppModel::instance()->fbx)
	{
		for (const auto &object_name : MapAppModel::instance()->fbx->object_names())
		{
			auto item = objects_list->add_item(object_name);
			if (first)
			{
				item->set_selected(true, false);
				first = false;
			}
			items[object_name] = item;
		}
	}
	*/
	/*
	for (const auto &object : MapAppModel::instance()->desc.objects)
	{
		auto &item = items[object.mesh_object];
		if (!item)
			item = objects_list->add_item(object.mesh_object);

		item->set_bold(true);

		if (first)
		{
			item->set_selected(true, false);
			first = false;
		}
	}
	*/

	//if (!objects_list->selection())
	//	object->set_hidden(true);
}

int ObjectsController::get_select_item_index()
{
	/*auto selection = objects_list->selection();
	if (selection)
	{
		std::string name = selection->text();

		const auto &objects = MapAppModel::instance()->desc.objects;
		for (size_t i = 0; i < objects.size(); i++)
		{
			if (objects[i].mesh_object == name)
				return (int)i;
		}
	}*/
	return -1;
}

void ObjectsController::update_object_fields()
{
	int index = get_select_item_index();

	if (index >= 0)
	{
		object->set_hidden(false);

		//const auto &object = MapAppModel::instance()->desc.objects[index];
	}
	else
	{
		object->set_hidden(true);
	}
}

void ObjectsController::objects_list_selection_changed()
{
	update_object_fields();
}

void ObjectsController::objects_list_selection_clicked()
{
	auto selection = objects_list->selection();
	if (selection)
	{
		/*
		if (selection->index >= MapAppModel::instance()->desc.objects.size())
		{
			MapAppModel::instance()->desc.objects.resize(selection->index + 1);
			objects_list->add_item("");
		}

		auto &object = MapAppModel::instance()->desc.objects[selection->index];
		object.name = selection->text();

		update_object_fields();
		*/
	}
}
