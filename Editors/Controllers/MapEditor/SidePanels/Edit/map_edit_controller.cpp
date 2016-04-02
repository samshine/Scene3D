
#include "precomp.h"
#include "map_edit_controller.h"
#include "Views/MapEditor/map_select_rollout.h"
#include "Views/MapEditor/map_create_rollout.h"
#include "Views/MapEditor/map_object_rollout.h"
#include "Views/MapEditor/map_object_list_rollout.h"
#include "Model/MapEditor/map_app_model.h"

using namespace uicore;

MapEditController::MapEditController()
{
	select = view->content_view()->add_child<MapSelectRolloutView>();
	create = view->content_view()->add_child<MapCreateRolloutView>();
	create_object = view->content_view()->add_child<MapObjectRolloutView>();
	objects = view->content_view()->add_child<MapObjectListRolloutView>();
	edit_object = view->content_view()->add_child<MapObjectRolloutView>();

	create_object->position->set_hidden();
	create_object->id->text_field->set_text(MapAppModel::instance()->create_object_template.id);
	create_object->type->text_field->set_text(MapAppModel::instance()->create_object_template.type);
	create_object->dir->text_field->set_text(MapAppModel::instance()->create_object_template.dir);
	create_object->up->text_field->set_text(MapAppModel::instance()->create_object_template.up);
	create_object->tilt->text_field->set_text(MapAppModel::instance()->create_object_template.tilt);
	create_object->scale->text_field->set_text(MapAppModel::instance()->create_object_template.scale);
	create_object->mesh->browse_field->set_text(PathHelp::basename(MapAppModel::instance()->create_object_template.mesh));
	create_object->fields->text_field->set_text(MapAppModel::instance()->create_object_template.fields.to_json());

	slots.connect(select->button_select->button->sig_clicked(), bind_member(this, &MapEditController::select_clicked));
	slots.connect(select->button_select_move->button->sig_clicked(), bind_member(this, &MapEditController::select_move_clicked));
	slots.connect(select->button_select_rotate->button->sig_clicked(), bind_member(this, &MapEditController::select_rotate_clicked));
	slots.connect(select->button_select_scale->button->sig_clicked(), bind_member(this, &MapEditController::select_scale_clicked));
	slots.connect(create->button_create_object->button->sig_clicked(), bind_member(this, &MapEditController::create_object_clicked));
	slots.connect(create->button_create_trigger->button->sig_clicked(), bind_member(this, &MapEditController::create_trigger_clicked));
	slots.connect(create->button_create_path_point->button->sig_clicked(), bind_member(this, &MapEditController::create_path_point_clicked));
	slots.connect(create->button_create_light_probe->button->sig_clicked(), bind_member(this, &MapEditController::create_light_probe_clicked));

	slots.connect(create_object->id->sig_value_changed(), this, &MapEditController::create_object_id_value_changed);
	slots.connect(create_object->type->sig_value_changed(), this, &MapEditController::create_object_type_value_changed);
	slots.connect(create_object->dir->sig_value_changed(), this, &MapEditController::create_object_dir_value_changed);
	slots.connect(create_object->up->sig_value_changed(), this, &MapEditController::create_object_up_value_changed);
	slots.connect(create_object->tilt->sig_value_changed(), this, &MapEditController::create_object_tilt_value_changed);
	slots.connect(create_object->scale->sig_value_changed(), this, &MapEditController::create_object_scale_value_changed);
	slots.connect(create_object->mesh->sig_browse(), this, &MapEditController::create_object_mesh_browse);
	slots.connect(create_object->fields->sig_value_changed(), this, &MapEditController::create_object_field_value_changed);

	slots.connect(objects->objects_list->sig_selection_changed(), this, &MapEditController::objects_list_selection_changed);

	slots.connect(edit_object->id->sig_value_changed(), this, &MapEditController::edit_object_id_value_changed);
	slots.connect(edit_object->type->sig_value_changed(), this, &MapEditController::edit_object_type_value_changed);
	slots.connect(edit_object->position->sig_value_changed(), this, &MapEditController::edit_object_position_value_changed);
	slots.connect(edit_object->dir->sig_value_changed(), this, &MapEditController::edit_object_dir_value_changed);
	slots.connect(edit_object->up->sig_value_changed(), this, &MapEditController::edit_object_up_value_changed);
	slots.connect(edit_object->tilt->sig_value_changed(), this, &MapEditController::edit_object_tilt_value_changed);
	slots.connect(edit_object->scale->sig_value_changed(), this, &MapEditController::edit_object_scale_value_changed);
	slots.connect(edit_object->mesh->sig_browse(), this, &MapEditController::edit_object_mesh_browse);
	slots.connect(edit_object->fields->sig_value_changed(), this, &MapEditController::edit_object_field_value_changed);

	slots.connect(MapAppModel::instance()->sig_load_finished, this, &MapEditController::update_objects);
	slots.connect(MapAppModel::instance()->sig_map_tool_changed, this, &MapEditController::map_tool_changed);
	slots.connect(MapAppModel::instance()->sig_object_updated, this, &MapEditController::object_updated);

	update_objects();
	map_tool_changed();
}

void MapEditController::map_tool_changed()
{
	select->button_select->button->set_state("selected", MapAppModel::instance()->tool == MapToolType::select);
	select->button_select_move->button->set_state("selected", MapAppModel::instance()->tool == MapToolType::select_move);
	select->button_select_rotate->button->set_state("selected", MapAppModel::instance()->tool == MapToolType::select_rotate);
	select->button_select_scale->button->set_state("selected", MapAppModel::instance()->tool == MapToolType::select_scale);
	create->button_create_object->button->set_state("selected", MapAppModel::instance()->tool == MapToolType::create_object);
	create->button_create_trigger->button->set_state("selected", MapAppModel::instance()->tool == MapToolType::create_trigger);
	create->button_create_path_point->button->set_state("selected", MapAppModel::instance()->tool == MapToolType::create_path_point);
	create->button_create_light_probe->button->set_state("selected", MapAppModel::instance()->tool == MapToolType::create_light_probe);
	create_object->set_hidden(MapAppModel::instance()->tool != MapToolType::create_object);
}

void MapEditController::select_clicked()
{
	MapAppModel::instance()->tool = MapToolType::select;
	MapAppModel::instance()->sig_map_tool_changed();
}

void MapEditController::select_move_clicked()
{
	MapAppModel::instance()->tool = MapToolType::select_move;
	MapAppModel::instance()->sig_map_tool_changed();
}

void MapEditController::select_rotate_clicked()
{
	MapAppModel::instance()->tool = MapToolType::select_rotate;
	MapAppModel::instance()->sig_map_tool_changed();
}

void MapEditController::select_scale_clicked()
{
	MapAppModel::instance()->tool = MapToolType::select_scale;
	MapAppModel::instance()->sig_map_tool_changed();
}

void MapEditController::create_object_clicked()
{
	MapAppModel::instance()->tool = MapToolType::create_object;
	MapAppModel::instance()->sig_map_tool_changed();
}

void MapEditController::create_trigger_clicked()
{
	MapAppModel::instance()->tool = MapToolType::create_trigger;
	MapAppModel::instance()->sig_map_tool_changed();
}

void MapEditController::create_path_point_clicked()
{
	MapAppModel::instance()->tool = MapToolType::create_path_point;
	MapAppModel::instance()->sig_map_tool_changed();
}

void MapEditController::create_light_probe_clicked()
{
	MapAppModel::instance()->tool = MapToolType::create_light_probe;
	MapAppModel::instance()->sig_map_tool_changed();
}

void MapEditController::create_object_id_value_changed()
{
	auto &object = MapAppModel::instance()->create_object_template;
	object.id = create_object->id->text_field->text();
}

void MapEditController::create_object_type_value_changed()
{
	auto &object = MapAppModel::instance()->create_object_template;
	object.type = create_object->type->text_field->text();
}

void MapEditController::create_object_dir_value_changed()
{
	auto &object = MapAppModel::instance()->create_object_template;
	object.dir = create_object->dir->text_field->text_float();
}

void MapEditController::create_object_up_value_changed()
{
	auto &object = MapAppModel::instance()->create_object_template;
	object.up = create_object->up->text_field->text_float();
}

void MapEditController::create_object_tilt_value_changed()
{
	auto &object = MapAppModel::instance()->create_object_template;
	object.tilt = create_object->tilt->text_field->text_float();
}

void MapEditController::create_object_scale_value_changed()
{
	auto &object = MapAppModel::instance()->create_object_template;
	object.scale = create_object->scale->text_field->text_float();
}

void MapEditController::create_object_mesh_browse()
{
	auto &object = MapAppModel::instance()->create_object_template;

	OpenFileDialog dialog(view.get());
	dialog.set_title("Select Mesh Model");
	dialog.add_filter("Model description files (*.modeldesc)", "*.modeldesc", true);
	dialog.add_filter("All files (*.*)", "*.*", false);
	dialog.set_filename(object.mesh);
	if (dialog.show())
	{
		object.mesh = dialog.filename();
		create_object->mesh->browse_field->set_text(PathHelp::basename(object.mesh));
	}
}

void MapEditController::create_object_field_value_changed()
{
	auto &object = MapAppModel::instance()->create_object_template;
	try
	{
		object.fields = JsonValue::parse(create_object->fields->text_field->text());
	}
	catch (const Exception &)
	{
	}
}

void MapEditController::object_updated(size_t index)
{
	const auto &object = MapAppModel::instance()->desc.objects[index];
	objects->objects_list->set_item_text(index, string_format("%1 (%2 - %3)", object.id, object.type, PathHelp::basename(object.mesh)));

	if (index == objects->objects_list->selected_item())
		update_object_fields();
}

void MapEditController::update_objects()
{
	objects->objects_list->clear();

	int i = 0;
	for (const auto &object : MapAppModel::instance()->desc.objects)
	{
		objects->objects_list->add_item(string_format("%1 (%2 - %3)", object.id, object.type, PathHelp::basename(object.mesh)));
		i++;
	}

	if (i > 0)
	{
		objects->objects_list->set_selected(0);
		edit_object->set_hidden(false);
		update_object_fields();
	}
	else
	{
		edit_object->set_hidden(true);
	}
}

void MapEditController::update_object_fields()
{
	int index = objects->objects_list->selected_item();

	if (index >= 0)
	{
		edit_object->set_hidden(false);

		const auto &object = MapAppModel::instance()->desc.objects[index];
		edit_object->id->text_field->set_text(object.id);
		edit_object->type->text_field->set_text(object.type);
		edit_object->position->input_x->set_text(Text::to_string(object.position.x));
		edit_object->position->input_y->set_text(Text::to_string(object.position.y));
		edit_object->position->input_z->set_text(Text::to_string(object.position.z));
		edit_object->dir->text_field->set_text(Text::to_string(object.dir));
		edit_object->up->text_field->set_text(Text::to_string(object.up));
		edit_object->tilt->text_field->set_text(Text::to_string(object.tilt));
		edit_object->scale->text_field->set_text(Text::to_string(object.scale));
		edit_object->mesh->browse_field->set_text(PathHelp::basename(object.mesh));
		edit_object->fields->text_field->set_text(object.fields.to_json());
	}
	else
	{
		edit_object->set_hidden(true);
	}
}

void MapEditController::objects_list_selection_changed()
{
	update_object_fields();
}

void MapEditController::edit_object_id_value_changed()
{
	auto selection = objects->objects_list->selected_item();
	if (selection != -1)
	{
		auto app_model = MapAppModel::instance();
		auto object = app_model->desc.objects.at(selection);
		object.id = edit_object->id->text_field->text();
		app_model->undo_system.execute<UpdateMapObjectCommand>(selection, object);
	}
}

void MapEditController::edit_object_type_value_changed()
{
	auto selection = objects->objects_list->selected_item();
	if (selection != -1)
	{
		auto app_model = MapAppModel::instance();
		auto object = app_model->desc.objects.at(selection);
		object.type = edit_object->type->text_field->text();
		app_model->undo_system.execute<UpdateMapObjectCommand>(selection, object);
	}
}

void MapEditController::edit_object_position_value_changed()
{
	auto selection = objects->objects_list->selected_item();
	if (selection != -1)
	{
		auto app_model = MapAppModel::instance();
		auto object = app_model->desc.objects.at(selection);
		object.position.x = edit_object->position->input_x->text_float();
		object.position.y = edit_object->position->input_y->text_float();
		object.position.z = edit_object->position->input_z->text_float();
		app_model->undo_system.execute<UpdateMapObjectCommand>(selection, object);
	}
}

void MapEditController::edit_object_dir_value_changed()
{
	auto selection = objects->objects_list->selected_item();
	if (selection != -1)
	{
		auto app_model = MapAppModel::instance();
		auto object = app_model->desc.objects.at(selection);
		object.dir = edit_object->dir->text_field->text_float();
		app_model->undo_system.execute<UpdateMapObjectCommand>(selection, object);
	}
}

void MapEditController::edit_object_up_value_changed()
{
	auto selection = objects->objects_list->selected_item();
	if (selection != -1)
	{
		auto app_model = MapAppModel::instance();
		auto object = app_model->desc.objects.at(selection);
		object.up = edit_object->up->text_field->text_float();
		app_model->undo_system.execute<UpdateMapObjectCommand>(selection, object);
	}
}

void MapEditController::edit_object_tilt_value_changed()
{
	auto selection = objects->objects_list->selected_item();
	if (selection != -1)
	{
		auto app_model = MapAppModel::instance();
		auto object = app_model->desc.objects.at(selection);
		object.tilt = edit_object->tilt->text_field->text_float();
		app_model->undo_system.execute<UpdateMapObjectCommand>(selection, object);
	}
}

void MapEditController::edit_object_scale_value_changed()
{
	auto selection = objects->objects_list->selected_item();
	if (selection != -1)
	{
		auto app_model = MapAppModel::instance();
		auto object = app_model->desc.objects.at(selection);
		object.scale = edit_object->scale->text_field->text_float();
		app_model->undo_system.execute<UpdateMapObjectCommand>(selection, object);
	}
}

void MapEditController::edit_object_mesh_browse()
{
	auto selection = objects->objects_list->selected_item();
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
			object.mesh = dialog.filename();
			MapAppModel::instance()->undo_system.execute<UpdateMapObjectCommand>(selection, object);
			edit_object->mesh->browse_field->set_text(PathHelp::basename(object.mesh));
		}
	}
}

void MapEditController::edit_object_field_value_changed()
{
	auto selection = objects->objects_list->selected_item();
	if (selection != -1)
	{
		auto app_model = MapAppModel::instance();
		auto object = app_model->desc.objects.at(selection);
		object.fields = JsonValue::parse(edit_object->fields->text_field->text());
		app_model->undo_system.execute<UpdateMapObjectCommand>(selection, object);
	}
}
