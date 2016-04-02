
#pragma once

#include "Controllers/SidePanel/side_panel_controller.h"

class LightsSidePanelView;

class LightsController : public SidePanelController
{
public:
	LightsController();

private:
	void update_lights();
	void update_light_fields();

	void lights_list_selection_changed();
	void lights_list_selection_clicked();

	int get_select_item_index();

	std::shared_ptr<LightsSidePanelView> panel;

	uicore::SlotContainer slots;
};
