
#pragma once

#include "Controllers/SidePanel/side_panel_controller.h"

class BonesSidePanelView;

class BonesController : public SidePanelController
{
public:
	BonesController();

private:
	void update_bones();
	void update_bone_fields();

	void bones_list_selection_changed();
	void bones_list_edit_saved();

	std::shared_ptr<BonesSidePanelView> panel;

	uicore::SlotContainer slots;
};
