
#pragma once

#include "Controllers/SidePanel/side_panel_controller.h"

class RolloutView;
class RolloutButton;

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

	clan::SlotContainer slots;

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
};
