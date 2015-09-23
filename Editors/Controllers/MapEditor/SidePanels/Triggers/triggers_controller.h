
#pragma once

#include "Controllers/SidePanel/side_panel_controller.h"

class RolloutView;
class RolloutTextFieldProperty;
class RolloutList;
class RolloutListItemView;

class TriggersController : public SidePanelController
{
public:
	TriggersController();

private:
	void update_triggers();
	void update_trigger_fields();

	void triggers_list_selection_changed();
	void triggers_list_selection_clicked();

	int get_select_item_index();

	std::shared_ptr<RolloutView> triggers;
	std::shared_ptr<RolloutList> triggers_list;

	std::shared_ptr<RolloutView> trigger;

	uicore::SlotContainer slots;
};
