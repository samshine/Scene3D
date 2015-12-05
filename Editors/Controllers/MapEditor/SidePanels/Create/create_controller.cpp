
#include "precomp.h"
#include "create_controller.h"
#include "Views/Rollout/rollout_view.h"
#include "Views/Rollout/rollout_button.h"
#include "Views/Rollout/rollout_text_field_property.h"
#include "Views/Rollout/rollout_browse_field_property.h"
#include "Model/MapEditor/map_app_model.h"

using namespace uicore;

CreateController::CreateController()
{
	select = std::make_shared<RolloutView>("SELECT");
	button_select = std::make_shared<RolloutButton>("SELECT");
	button_select_move = std::make_shared<RolloutButton>("MOVE");
	button_select_rotate = std::make_shared<RolloutButton>("ROTATE");
	button_select_scale = std::make_shared<RolloutButton>("SCALE");
	create = std::make_shared<RolloutView>("CREATE");
	button_create_object = std::make_shared<RolloutButton>("OBJECT");
	button_create_trigger = std::make_shared<RolloutButton>("TRIGGER");
	button_create_path_point = std::make_shared<RolloutButton>("PATH POINT");
	button_create_light_probe = std::make_shared<RolloutButton>("LIGHT PROBE");

	object = std::make_shared<RolloutView>("OBJECT");
	id = std::make_shared<RolloutTextFieldProperty>("ID");
	type = std::make_shared<RolloutTextFieldProperty>("TYPE");
	dir = std::make_shared<RolloutTextFieldProperty>("DIR");
	up = std::make_shared<RolloutTextFieldProperty>("UP");
	tilt = std::make_shared<RolloutTextFieldProperty>("TILT");
	scale = std::make_shared<RolloutTextFieldProperty>("SCALE");
	mesh = std::make_shared<RolloutBrowseFieldProperty>("MODEL");
	fields = std::make_shared<RolloutTextFieldProperty>("FIELDS");

	id->text_field->set_text(MapAppModel::instance()->create_object_template.id);
	type->text_field->set_text(MapAppModel::instance()->create_object_template.type);
	dir->text_field->set_text(MapAppModel::instance()->create_object_template.dir);
	up->text_field->set_text(MapAppModel::instance()->create_object_template.up);
	tilt->text_field->set_text(MapAppModel::instance()->create_object_template.tilt);
	scale->text_field->set_text(MapAppModel::instance()->create_object_template.scale);
	mesh->browse_field->set_text(PathHelp::get_basename(MapAppModel::instance()->create_object_template.mesh));
	fields->text_field->set_text(MapAppModel::instance()->create_object_template.fields.to_json());

	select->content->add_subview(button_select);
	select->content->add_subview(button_select_move);
	select->content->add_subview(button_select_rotate);
	select->content->add_subview(button_select_scale);
	create->content->add_subview(button_create_object);
	create->content->add_subview(button_create_trigger);
	create->content->add_subview(button_create_path_point);
	create->content->add_subview(button_create_light_probe);

	object->content->add_subview(id);
	object->content->add_subview(type);
	object->content->add_subview(dir);
	object->content->add_subview(up);
	object->content->add_subview(tilt);
	object->content->add_subview(scale);
	object->content->add_subview(mesh);
	object->content->add_subview(fields);

	view->content_view()->add_subview(select);
	view->content_view()->add_subview(create);
	view->content_view()->add_subview(object);

	button_select->button->func_clicked() = bind_member(this, &CreateController::select_clicked);
	button_select_move->button->func_clicked() = bind_member(this, &CreateController::select_move_clicked);
	button_select_rotate->button->func_clicked() = bind_member(this, &CreateController::select_rotate_clicked);
	button_select_scale->button->func_clicked() = bind_member(this, &CreateController::select_scale_clicked);
	button_create_object->button->func_clicked() = bind_member(this, &CreateController::create_object_clicked);
	button_create_trigger->button->func_clicked() = bind_member(this, &CreateController::create_trigger_clicked);
	button_create_path_point->button->func_clicked() = bind_member(this, &CreateController::create_path_point_clicked);
	button_create_light_probe->button->func_clicked() = bind_member(this, &CreateController::create_light_probe_clicked);

	slots.connect(id->sig_value_changed(), this, &CreateController::id_value_changed);
	slots.connect(type->sig_value_changed(), this, &CreateController::type_value_changed);
	slots.connect(dir->sig_value_changed(), this, &CreateController::dir_value_changed);
	slots.connect(up->sig_value_changed(), this, &CreateController::up_value_changed);
	slots.connect(tilt->sig_value_changed(), this, &CreateController::tilt_value_changed);
	slots.connect(scale->sig_value_changed(), this, &CreateController::scale_value_changed);
	slots.connect(mesh->sig_browse(), this, &CreateController::mesh_browse);
	slots.connect(fields->sig_value_changed(), this, &CreateController::field_value_changed);

	slots.connect(MapAppModel::instance()->sig_map_tool_changed, this, &CreateController::map_tool_changed);

	map_tool_changed();
}

void CreateController::map_tool_changed()
{
	button_select->button->set_state("selected", MapAppModel::instance()->tool == MapToolType::select);
	button_select_move->button->set_state("selected", MapAppModel::instance()->tool == MapToolType::select_move);
	button_select_rotate->button->set_state("selected", MapAppModel::instance()->tool == MapToolType::select_rotate);
	button_select_scale->button->set_state("selected", MapAppModel::instance()->tool == MapToolType::select_scale);
	button_create_object->button->set_state("selected", MapAppModel::instance()->tool == MapToolType::create_object);
	button_create_trigger->button->set_state("selected", MapAppModel::instance()->tool == MapToolType::create_trigger);
	button_create_path_point->button->set_state("selected", MapAppModel::instance()->tool == MapToolType::create_path_point);
	button_create_light_probe->button->set_state("selected", MapAppModel::instance()->tool == MapToolType::create_light_probe);
	object->set_hidden(MapAppModel::instance()->tool != MapToolType::create_object);
}

void CreateController::select_clicked()
{
	MapAppModel::instance()->tool = MapToolType::select;
	MapAppModel::instance()->sig_map_tool_changed();
	map_tool_changed();
}

void CreateController::select_move_clicked()
{
	MapAppModel::instance()->tool = MapToolType::select_move;
	MapAppModel::instance()->sig_map_tool_changed();
}

void CreateController::select_rotate_clicked()
{
	MapAppModel::instance()->tool = MapToolType::select_rotate;
	MapAppModel::instance()->sig_map_tool_changed();
}

void CreateController::select_scale_clicked()
{
	MapAppModel::instance()->tool = MapToolType::select_scale;
	MapAppModel::instance()->sig_map_tool_changed();
}

void CreateController::create_object_clicked()
{
	MapAppModel::instance()->tool = MapToolType::create_object;
	MapAppModel::instance()->sig_map_tool_changed();
}

void CreateController::create_trigger_clicked()
{
	MapAppModel::instance()->tool = MapToolType::create_trigger;
	MapAppModel::instance()->sig_map_tool_changed();
}

void CreateController::create_path_point_clicked()
{
	MapAppModel::instance()->tool = MapToolType::create_path_point;
	MapAppModel::instance()->sig_map_tool_changed();
}

void CreateController::create_light_probe_clicked()
{
	MapAppModel::instance()->tool = MapToolType::create_light_probe;
	MapAppModel::instance()->sig_map_tool_changed();
}

void CreateController::id_value_changed()
{
	auto &object = MapAppModel::instance()->create_object_template;
	object.id = id->text_field->text();
}

void CreateController::type_value_changed()
{
	auto &object = MapAppModel::instance()->create_object_template;
	object.type = type->text_field->text();
}

void CreateController::dir_value_changed()
{
	auto &object = MapAppModel::instance()->create_object_template;
	object.dir = dir->text_field->text_float();
}

void CreateController::up_value_changed()
{
	auto &object = MapAppModel::instance()->create_object_template;
	object.up = up->text_field->text_float();
}

void CreateController::tilt_value_changed()
{
	auto &object = MapAppModel::instance()->create_object_template;
	object.tilt = tilt->text_field->text_float();
}

void CreateController::scale_value_changed()
{
	auto &object = MapAppModel::instance()->create_object_template;
	object.scale = scale->text_field->text_float();
}

void CreateController::mesh_browse()
{
	auto &object = MapAppModel::instance()->create_object_template;

	OpenFileDialog dialog(view.get());
	dialog.set_title("Select Mesh Model");
	dialog.add_filter("Model description files (*.modeldesc)", "*.modeldesc", true);
	dialog.add_filter("All files (*.*)", "*.*", false);
	dialog.set_filename(object.mesh);
	if (dialog.show())
	{
		object.mesh = dialog.get_filename();
		mesh->browse_field->set_text(PathHelp::get_basename(object.mesh));
	}
}

void CreateController::field_value_changed()
{
	auto &object = MapAppModel::instance()->create_object_template;
	try
	{
		object.fields = JsonValue::parse(fields->text_field->text());
	}
	catch (const Exception &)
	{
	}
}
