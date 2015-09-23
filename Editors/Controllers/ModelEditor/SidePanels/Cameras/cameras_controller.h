
#pragma once

#include "Controllers/SidePanel/side_panel_controller.h"

class RolloutView;
class RolloutTextFieldProperty;
class RolloutList;
class RolloutListItemView;

class CamerasController : public SidePanelController
{
public:
	CamerasController();

private:
	void update_cameras();
	void update_camera_fields();

	void cameras_list_selection_changed();
	void cameras_list_selection_clicked();

	int get_select_item_index();

	std::shared_ptr<RolloutView> cameras;
	std::shared_ptr<RolloutList> cameras_list;

	std::shared_ptr<RolloutView> camera;

	uicore::SlotContainer slots;
};
