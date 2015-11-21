
#include "precomp.h"
#include "objects_controller.h"
#include "Views/Rollout/rollout_view.h"
#include "Views/Rollout/rollout_list.h"
#include "Views/Rollout/rollout_text_field_property.h"
#include "Views/Rollout/rollout_position_property.h"
#include "Views/Rollout/rollout_browse_field_property.h"
#include "Model/MapEditor/map_app_model.h"

using namespace uicore;

ObjectsController::ObjectsController()
{
	objects = std::make_shared<RolloutView>("OBJECTS");
	object = std::make_shared<RolloutView>("OBJECT");
	id = std::make_shared<RolloutTextFieldProperty>("ID");
	type = std::make_shared<RolloutTextFieldProperty>("TYPE");
	position = std::make_shared<RolloutPositionProperty>("POSITION");
	dir = std::make_shared<RolloutTextFieldProperty>("DIR");
	up = std::make_shared<RolloutTextFieldProperty>("UP");
	tilt = std::make_shared<RolloutTextFieldProperty>("TILT");
	scale = std::make_shared<RolloutTextFieldProperty>("SCALE");
	mesh = std::make_shared<RolloutBrowseFieldProperty>("MODEL");
	fields = std::make_shared<RolloutTextFieldProperty>("FIELDS");

	view->content_view()->add_subview(objects);
	view->content_view()->add_subview(object);

	objects_list = std::make_shared<RolloutList>();
	objects_list->set_allow_edit(false);

	objects->content->add_subview(objects_list);

	object->content->add_subview(id);
	object->content->add_subview(type);
	object->content->add_subview(position);
	object->content->add_subview(dir);
	object->content->add_subview(up);
	object->content->add_subview(tilt);
	object->content->add_subview(scale);
	object->content->add_subview(mesh);
	object->content->add_subview(fields);

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

	int i = 0;
	for (const auto &object : MapAppModel::instance()->desc.objects)
	{
		auto &item = items[Text::to_string(i)];
		if (!item)
			item = objects_list->add_item(string_format("%1 (%2)", i, object.type));

		item->set_bold(true);

		if (first)
		{
			item->set_selected(true, false);
			first = false;
		}
		i++;
	}

	if (!objects_list->selection())
		object->set_hidden(true);
}

int ObjectsController::get_select_item_index()
{
	auto selection = objects_list->selection();
	if (selection)
	{
		return selection->index;
	}
	return -1;
}

void ObjectsController::update_object_fields()
{
	int index = get_select_item_index();

	if (index >= 0)
	{
		object->set_hidden(false);

		const auto &object = MapAppModel::instance()->desc.objects[index];
		id->text_field->set_text(object.id);
		type->text_field->set_text(object.type);
		position->input_x->set_text(Text::to_string(object.position.x));
		position->input_y->set_text(Text::to_string(object.position.y));
		position->input_z->set_text(Text::to_string(object.position.z));
		dir->text_field->set_text(Text::to_string(object.dir));
		up->text_field->set_text(Text::to_string(object.up));
		tilt->text_field->set_text(Text::to_string(object.tilt));
		scale->text_field->set_text(Text::to_string(object.scale));
		mesh->browse_field->set_text(PathHelp::get_basename(object.mesh));
		fields->text_field->set_text(object.fields.to_json());
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
