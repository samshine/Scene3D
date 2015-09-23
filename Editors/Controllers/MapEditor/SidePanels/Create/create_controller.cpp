
#include "precomp.h"
#include "create_controller.h"
#include "Views/Rollout/rollout_view.h"
#include "Views/Rollout/rollout_button.h"
#include "Views/Rollout/rollout_text_field_property.h"
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

	select->content->add_subview(button_select);
	select->content->add_subview(button_select_move);
	select->content->add_subview(button_select_rotate);
	select->content->add_subview(button_select_scale);
	create->content->add_subview(button_create_object);
	create->content->add_subview(button_create_trigger);
	create->content->add_subview(button_create_path_point);
	create->content->add_subview(button_create_light_probe);

	content_view()->add_subview(select);
	content_view()->add_subview(create);

	button_select->button->func_clicked() = bind_member(this, &CreateController::select_clicked);
	button_select_move->button->func_clicked() = bind_member(this, &CreateController::select_move_clicked);
	button_select_rotate->button->func_clicked() = bind_member(this, &CreateController::select_rotate_clicked);
	button_select_scale->button->func_clicked() = bind_member(this, &CreateController::select_scale_clicked);
	button_create_object->button->func_clicked() = bind_member(this, &CreateController::create_object_clicked);
	button_create_trigger->button->func_clicked() = bind_member(this, &CreateController::create_trigger_clicked);
	button_create_path_point->button->func_clicked() = bind_member(this, &CreateController::create_path_point_clicked);
	button_create_light_probe->button->func_clicked() = bind_member(this, &CreateController::create_light_probe_clicked);

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
