
#include "precomp.h"
#include "objects_controller.h"
#include "Views/Rollout/rollout_view.h"
#include "Views/Rollout/rollout_list.h"
#include "Views/Rollout/rollout_text_field_property.h"
#include "Views/Rollout/rollout_position_property.h"
#include "Views/Rollout/rollout_browse_field_property.h"
#include "Model/MapEditor/map_app_model.h"
#include "Model/MapEditor/Commands/update_map_object_command.h"

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

	slots.connect(id->sig_value_changed(), this, &ObjectsController::id_value_changed);
	slots.connect(type->sig_value_changed(), this, &ObjectsController::type_value_changed);
	slots.connect(position->sig_value_changed(), this, &ObjectsController::position_value_changed);
	slots.connect(dir->sig_value_changed(), this, &ObjectsController::dir_value_changed);
	slots.connect(up->sig_value_changed(), this, &ObjectsController::up_value_changed);
	slots.connect(tilt->sig_value_changed(), this, &ObjectsController::tilt_value_changed);
	slots.connect(scale->sig_value_changed(), this, &ObjectsController::scale_value_changed);
	slots.connect(mesh->sig_browse(), this, &ObjectsController::mesh_browse);
	slots.connect(fields->sig_value_changed(), this, &ObjectsController::field_value_changed);

	slots.connect(MapAppModel::instance()->sig_load_finished, [this]() { update_objects(); });

	update_objects();
}

void ObjectsController::update_objects()
{
	objects_list->clear();

	int i = 0;
	for (const auto &object : MapAppModel::instance()->desc.objects)
	{
		objects_list->add_item(string_format("%1 (%2 - %3)", object.id, object.type, PathHelp::get_basename(object.mesh)));
		i++;
	}

	if (i > 0)
	{
		objects_list->set_selected(0);
		object->set_hidden(false);
	}
	else
	{
		object->set_hidden(true);
	}
}

int ObjectsController::get_select_item_index()
{
	return objects_list->selected_item();
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
	auto selection = objects_list->selected_item();
	if (selection != -1)
	{
		/*
		if (selection->index >= MapAppModel::instance()->desc.objects.size())
		{
			MapAppModel::instance()->desc.objects.resize(selection->index + 1);
			objects_list->add_item("");
		}

		auto &object = MapAppModel::instance()->desc.objects[selection->index];
		object.name = objects_list->item_text(selection);

		update_object_fields();
		*/
	}
}

void ObjectsController::id_value_changed()
{
	auto selection = objects_list->selected_item();
	if (selection != -1)
	{
		auto app_model = MapAppModel::instance();
		auto object = app_model->desc.objects.at(selection);
		object.id = id->text_field->text();
		app_model->undo_system.execute<UpdateMapObjectCommand>(selection, object);
	}
}

void ObjectsController::type_value_changed()
{
	auto selection = objects_list->selected_item();
	if (selection != -1)
	{
		auto app_model = MapAppModel::instance();
		auto object = app_model->desc.objects.at(selection);
		object.type = type->text_field->text();
		app_model->undo_system.execute<UpdateMapObjectCommand>(selection, object);
	}
}

void ObjectsController::position_value_changed()
{
	auto selection = objects_list->selected_item();
	if (selection != -1)
	{
		auto app_model = MapAppModel::instance();
		auto object = app_model->desc.objects.at(selection);
		object.position.x = position->input_x->text_float();
		object.position.y = position->input_y->text_float();
		object.position.z = position->input_z->text_float();
		app_model->undo_system.execute<UpdateMapObjectCommand>(selection, object);
	}
}

void ObjectsController::dir_value_changed()
{
	auto selection = objects_list->selected_item();
	if (selection != -1)
	{
		auto app_model = MapAppModel::instance();
		auto object = app_model->desc.objects.at(selection);
		object.dir = dir->text_field->text_float();
		app_model->undo_system.execute<UpdateMapObjectCommand>(selection, object);
	}
}

void ObjectsController::up_value_changed()
{
	auto selection = objects_list->selected_item();
	if (selection != -1)
	{
		auto app_model = MapAppModel::instance();
		auto object = app_model->desc.objects.at(selection);
		object.up = up->text_field->text_float();
		app_model->undo_system.execute<UpdateMapObjectCommand>(selection, object);
	}
}

void ObjectsController::tilt_value_changed()
{
	auto selection = objects_list->selected_item();
	if (selection != -1)
	{
		auto app_model = MapAppModel::instance();
		auto object = app_model->desc.objects.at(selection);
		object.tilt = tilt->text_field->text_float();
		app_model->undo_system.execute<UpdateMapObjectCommand>(selection, object);
	}
}

void ObjectsController::scale_value_changed()
{
	auto selection = objects_list->selected_item();
	if (selection != -1)
	{
		auto app_model = MapAppModel::instance();
		auto object = app_model->desc.objects.at(selection);
		object.scale = scale->text_field->text_float();
		app_model->undo_system.execute<UpdateMapObjectCommand>(selection, object);
	}
}

void ObjectsController::mesh_browse()
{
	auto selection = objects_list->selected_item();
	if (selection != -1)
	{
		auto app_model = MapAppModel::instance();
		auto object = app_model->desc.objects.at(selection);

		OpenFileDialog dialog(view.get());
		dialog.set_title("Select Mesh Model");
		dialog.add_filter("Model description files (*.modeldesc)", "*.modeldesc", true);
		dialog.add_filter("All files (*.*)", "*.*", false);
		dialog.set_filename(object.mesh);
		if (dialog.show())
		{
			object.mesh = dialog.get_filename();
			MapAppModel::instance()->undo_system.execute<UpdateMapObjectCommand>(selection, object);
			mesh->browse_field->set_text(PathHelp::get_basename(object.mesh));
		}
	}
}

void ObjectsController::field_value_changed()
{
	auto selection = objects_list->selected_item();
	if (selection != -1)
	{
		auto app_model = MapAppModel::instance();
		auto object = app_model->desc.objects.at(selection);
		object.fields = JsonValue::parse(fields->text_field->text());
		app_model->undo_system.execute<UpdateMapObjectCommand>(selection, object);
	}
}
