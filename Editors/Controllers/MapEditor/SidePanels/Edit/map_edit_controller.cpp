
#include "precomp.h"
#include "map_edit_controller.h"
#include "Model/MapEditor/map_app_model.h"
#include "Views/MapEditor/map_edit_sidepanel.h"

using namespace uicore;

MapEditController::MapEditController()
{
	panel = view->content_view()->add_child<MapEditSidePanelView>();

	panel->create_object->position->set_hidden();
	panel->create_object->id->text_field->set_text(MapAppModel::instance()->create_object_template.id);
	panel->create_object->type->text_field->set_text(MapAppModel::instance()->create_object_template.type);
	panel->create_object->dir->text_field->set_text(MapAppModel::instance()->create_object_template.dir);
	panel->create_object->up->text_field->set_text(MapAppModel::instance()->create_object_template.up);
	panel->create_object->tilt->text_field->set_text(MapAppModel::instance()->create_object_template.tilt);
	panel->create_object->scale->text_field->set_text(MapAppModel::instance()->create_object_template.scale);
	panel->create_object->mesh->browse_field->set_text(PathHelp::basename(MapAppModel::instance()->create_object_template.mesh));
	panel->create_object->fields->text_field->set_text(MapAppModel::instance()->create_object_template.fields.to_json());

	slots.connect(panel->select->button_select->button->sig_clicked(), bind_member(this, &MapEditController::select_clicked));
	slots.connect(panel->select->button_select_move->button->sig_clicked(), bind_member(this, &MapEditController::select_move_clicked));
	slots.connect(panel->select->button_select_rotate->button->sig_clicked(), bind_member(this, &MapEditController::select_rotate_clicked));
	slots.connect(panel->select->button_select_scale->button->sig_clicked(), bind_member(this, &MapEditController::select_scale_clicked));
	slots.connect(panel->create->button_create_object->button->sig_clicked(), bind_member(this, &MapEditController::create_object_clicked));
	slots.connect(panel->create->button_create_trigger->button->sig_clicked(), bind_member(this, &MapEditController::create_trigger_clicked));
	slots.connect(panel->create->button_create_path_point->button->sig_clicked(), bind_member(this, &MapEditController::create_path_point_clicked));
	slots.connect(panel->create->button_create_light_probe->button->sig_clicked(), bind_member(this, &MapEditController::create_light_probe_clicked));

	slots.connect(panel->create_object->id->sig_value_changed(), this, &MapEditController::create_object_id_value_changed);
	slots.connect(panel->create_object->type->sig_value_changed(), this, &MapEditController::create_object_type_value_changed);
	slots.connect(panel->create_object->dir->sig_value_changed(), this, &MapEditController::create_object_dir_value_changed);
	slots.connect(panel->create_object->up->sig_value_changed(), this, &MapEditController::create_object_up_value_changed);
	slots.connect(panel->create_object->tilt->sig_value_changed(), this, &MapEditController::create_object_tilt_value_changed);
	slots.connect(panel->create_object->scale->sig_value_changed(), this, &MapEditController::create_object_scale_value_changed);
	slots.connect(panel->create_object->mesh->sig_browse(), this, &MapEditController::create_object_mesh_browse);
	slots.connect(panel->create_object->fields->sig_value_changed(), this, &MapEditController::create_object_field_value_changed);

	slots.connect(panel->objects->objects_list->sig_selection_changed(), this, &MapEditController::objects_list_selection_changed);

	slots.connect(panel->edit_object->id->sig_value_changed(), this, &MapEditController::edit_object_id_value_changed);
	slots.connect(panel->edit_object->type->sig_value_changed(), this, &MapEditController::edit_object_type_value_changed);
	slots.connect(panel->edit_object->position->sig_value_changed(), this, &MapEditController::edit_object_position_value_changed);
	slots.connect(panel->edit_object->dir->sig_value_changed(), this, &MapEditController::edit_object_dir_value_changed);
	slots.connect(panel->edit_object->up->sig_value_changed(), this, &MapEditController::edit_object_up_value_changed);
	slots.connect(panel->edit_object->tilt->sig_value_changed(), this, &MapEditController::edit_object_tilt_value_changed);
	slots.connect(panel->edit_object->scale->sig_value_changed(), this, &MapEditController::edit_object_scale_value_changed);
	slots.connect(panel->edit_object->mesh->sig_browse(), this, &MapEditController::edit_object_mesh_browse);
	slots.connect(panel->edit_object->fields->sig_value_changed(), this, &MapEditController::edit_object_field_value_changed);

	slots.connect(MapAppModel::instance()->sig_load_finished, this, &MapEditController::update_objects);
	slots.connect(MapAppModel::instance()->sig_map_tool_changed, this, &MapEditController::map_tool_changed);
	slots.connect(MapAppModel::instance()->sig_object_updated, this, &MapEditController::object_updated);

	update_objects();
	map_tool_changed();
}

void MapEditController::map_tool_changed()
{
	panel->select->button_select->button->set_state("selected", MapAppModel::instance()->tool == MapToolType::select);
	panel->select->button_select_move->button->set_state("selected", MapAppModel::instance()->tool == MapToolType::select_move);
	panel->select->button_select_rotate->button->set_state("selected", MapAppModel::instance()->tool == MapToolType::select_rotate);
	panel->select->button_select_scale->button->set_state("selected", MapAppModel::instance()->tool == MapToolType::select_scale);
	panel->create->button_create_object->button->set_state("selected", MapAppModel::instance()->tool == MapToolType::create_object);
	panel->create->button_create_trigger->button->set_state("selected", MapAppModel::instance()->tool == MapToolType::create_trigger);
	panel->create->button_create_path_point->button->set_state("selected", MapAppModel::instance()->tool == MapToolType::create_path_point);
	panel->create->button_create_light_probe->button->set_state("selected", MapAppModel::instance()->tool == MapToolType::create_light_probe);
	panel->create_object->set_hidden(MapAppModel::instance()->tool != MapToolType::create_object);
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
	object.id = panel->create_object->id->text_field->text();
}

void MapEditController::create_object_type_value_changed()
{
	auto &object = MapAppModel::instance()->create_object_template;
	object.type = panel->create_object->type->text_field->text();
}

void MapEditController::create_object_dir_value_changed()
{
	auto &object = MapAppModel::instance()->create_object_template;
	object.dir = panel->create_object->dir->text_field->text_float();
}

void MapEditController::create_object_up_value_changed()
{
	auto &object = MapAppModel::instance()->create_object_template;
	object.up = panel->create_object->up->text_field->text_float();
}

void MapEditController::create_object_tilt_value_changed()
{
	auto &object = MapAppModel::instance()->create_object_template;
	object.tilt = panel->create_object->tilt->text_field->text_float();
}

void MapEditController::create_object_scale_value_changed()
{
	auto &object = MapAppModel::instance()->create_object_template;
	object.scale = panel->create_object->scale->text_field->text_float();
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
		panel->create_object->mesh->browse_field->set_text(PathHelp::basename(object.mesh));
	}
}

void MapEditController::create_object_field_value_changed()
{
	auto &object = MapAppModel::instance()->create_object_template;
	try
	{
		object.fields = JsonValue::parse(panel->create_object->fields->text_field->text());
	}
	catch (const Exception &)
	{
	}
}

void MapEditController::object_updated(size_t index)
{
	const auto &object = MapAppModel::instance()->desc.objects[index];
	panel->objects->objects_list->set_item_text(index, string_format("%1 (%2 - %3)", object.id, object.type, PathHelp::basename(object.mesh)));

	if (index == panel->objects->objects_list->selected_item())
		update_object_fields();
}

void MapEditController::update_objects()
{
	panel->objects->objects_list->clear();

	int i = 0;
	for (const auto &object : MapAppModel::instance()->desc.objects)
	{
		panel->objects->objects_list->add_item(string_format("%1 (%2 - %3)", object.id, object.type, PathHelp::basename(object.mesh)));
		i++;
	}

	if (i > 0)
	{
		panel->objects->objects_list->set_selected(0);
		panel->edit_object->set_hidden(false);
		update_object_fields();
	}
	else
	{
		panel->edit_object->set_hidden(true);
	}
}

void MapEditController::update_object_fields()
{
	int index = panel->objects->objects_list->selected_item();

	if (index >= 0)
	{
		panel->edit_object->set_hidden(false);

		const auto &object = MapAppModel::instance()->desc.objects[index];
		panel->edit_object->id->text_field->set_text(object.id);
		panel->edit_object->type->text_field->set_text(object.type);
		panel->edit_object->position->input_x->set_text(Text::to_string(object.position.x));
		panel->edit_object->position->input_y->set_text(Text::to_string(object.position.y));
		panel->edit_object->position->input_z->set_text(Text::to_string(object.position.z));
		panel->edit_object->dir->text_field->set_text(Text::to_string(object.dir));
		panel->edit_object->up->text_field->set_text(Text::to_string(object.up));
		panel->edit_object->tilt->text_field->set_text(Text::to_string(object.tilt));
		panel->edit_object->scale->text_field->set_text(Text::to_string(object.scale));
		panel->edit_object->mesh->browse_field->set_text(PathHelp::basename(object.mesh));
		panel->edit_object->fields->text_field->set_text(object.fields.to_json());
	}
	else
	{
		panel->edit_object->set_hidden(true);
	}
}

void MapEditController::objects_list_selection_changed()
{
	update_object_fields();
}

void MapEditController::edit_object_id_value_changed()
{
	auto selection = panel->objects->objects_list->selected_item();
	if (selection != -1)
	{
		auto app_model = MapAppModel::instance();
		auto object = app_model->desc.objects.at(selection);
		object.id = panel->edit_object->id->text_field->text();
		app_model->undo_system.execute<UpdateMapObjectCommand>(selection, object);
	}
}

void MapEditController::edit_object_type_value_changed()
{
	auto selection = panel->objects->objects_list->selected_item();
	if (selection != -1)
	{
		auto app_model = MapAppModel::instance();
		auto object = app_model->desc.objects.at(selection);
		object.type = panel->edit_object->type->text_field->text();
		app_model->undo_system.execute<UpdateMapObjectCommand>(selection, object);
	}
}

void MapEditController::edit_object_position_value_changed()
{
	auto selection = panel->objects->objects_list->selected_item();
	if (selection != -1)
	{
		auto app_model = MapAppModel::instance();
		auto object = app_model->desc.objects.at(selection);
		object.position.x = panel->edit_object->position->input_x->text_float();
		object.position.y = panel->edit_object->position->input_y->text_float();
		object.position.z = panel->edit_object->position->input_z->text_float();
		app_model->undo_system.execute<UpdateMapObjectCommand>(selection, object);
	}
}

void MapEditController::edit_object_dir_value_changed()
{
	auto selection = panel->objects->objects_list->selected_item();
	if (selection != -1)
	{
		auto app_model = MapAppModel::instance();
		auto object = app_model->desc.objects.at(selection);
		object.dir = panel->edit_object->dir->text_field->text_float();
		app_model->undo_system.execute<UpdateMapObjectCommand>(selection, object);
	}
}

void MapEditController::edit_object_up_value_changed()
{
	auto selection = panel->objects->objects_list->selected_item();
	if (selection != -1)
	{
		auto app_model = MapAppModel::instance();
		auto object = app_model->desc.objects.at(selection);
		object.up = panel->edit_object->up->text_field->text_float();
		app_model->undo_system.execute<UpdateMapObjectCommand>(selection, object);
	}
}

void MapEditController::edit_object_tilt_value_changed()
{
	auto selection = panel->objects->objects_list->selected_item();
	if (selection != -1)
	{
		auto app_model = MapAppModel::instance();
		auto object = app_model->desc.objects.at(selection);
		object.tilt = panel->edit_object->tilt->text_field->text_float();
		app_model->undo_system.execute<UpdateMapObjectCommand>(selection, object);
	}
}

void MapEditController::edit_object_scale_value_changed()
{
	auto selection = panel->objects->objects_list->selected_item();
	if (selection != -1)
	{
		auto app_model = MapAppModel::instance();
		auto object = app_model->desc.objects.at(selection);
		object.scale = panel->edit_object->scale->text_field->text_float();
		app_model->undo_system.execute<UpdateMapObjectCommand>(selection, object);
	}
}

void MapEditController::edit_object_mesh_browse()
{
	auto selection = panel->objects->objects_list->selected_item();
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
			panel->edit_object->mesh->browse_field->set_text(PathHelp::basename(object.mesh));
		}
	}
}

void MapEditController::edit_object_field_value_changed()
{
	auto selection = panel->objects->objects_list->selected_item();
	if (selection != -1)
	{
		auto app_model = MapAppModel::instance();
		auto object = app_model->desc.objects.at(selection);
		object.fields = JsonValue::parse(panel->edit_object->fields->text_field->text());
		app_model->undo_system.execute<UpdateMapObjectCommand>(selection, object);
	}
}
