
#pragma once

#include "Controllers/SidePanel/side_panel_controller.h"

class RolloutView;
class RolloutButton;
class RolloutTextFieldProperty;
class RolloutBrowseFieldProperty;

class CreateController : public SidePanelController
{
public:
	CreateController();

private:
	void map_tool_changed();

	void select_clicked();
	void select_move_clicked();
	void select_rotate_clicked();
	void select_scale_clicked();
	void create_object_clicked();
	void create_trigger_clicked();
	void create_path_point_clicked();
	void create_light_probe_clicked();

	void id_value_changed();
	void type_value_changed();
	void dir_value_changed();
	void up_value_changed();
	void tilt_value_changed();
	void scale_value_changed();
	void mesh_browse();
	void field_value_changed();

	uicore::SlotContainer slots;

	std::shared_ptr<RolloutView> select;
	std::shared_ptr<RolloutView> create;
	std::shared_ptr<RolloutButton> button_select;
	std::shared_ptr<RolloutButton> button_select_move;
	std::shared_ptr<RolloutButton> button_select_rotate;
	std::shared_ptr<RolloutButton> button_select_scale;
	std::shared_ptr<RolloutButton> button_create_object;
	std::shared_ptr<RolloutButton> button_create_trigger;
	std::shared_ptr<RolloutButton> button_create_path_point;
	std::shared_ptr<RolloutButton> button_create_light_probe;

	std::shared_ptr<RolloutView> object;
	std::shared_ptr<RolloutTextFieldProperty> id;
	std::shared_ptr<RolloutTextFieldProperty> type;
	std::shared_ptr<RolloutTextFieldProperty> dir;
	std::shared_ptr<RolloutTextFieldProperty> up;
	std::shared_ptr<RolloutTextFieldProperty> tilt;
	std::shared_ptr<RolloutTextFieldProperty> scale;
	std::shared_ptr<RolloutBrowseFieldProperty> mesh;
	std::shared_ptr<RolloutTextFieldProperty> fields;
};
