
#pragma once

#include "Controllers/SidePanel/side_panel_controller.h"

class RolloutView;
class RolloutTextFieldProperty;
class RolloutList;
class RolloutListItemView;

class BonesController : public SidePanelController
{
public:
	BonesController();

private:
	void update_bones();
	void update_bone_fields();

	void bones_list_selection_changed();
	void bones_list_selection_clicked();

	int get_select_item_index();

	std::shared_ptr<RolloutView> bones;
	std::shared_ptr<RolloutList> bones_list;

	std::shared_ptr<RolloutView> bone;

	uicore::SlotContainer slots;
};
