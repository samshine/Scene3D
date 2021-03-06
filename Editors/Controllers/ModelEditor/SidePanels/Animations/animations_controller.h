
#pragma once

#include "Controllers/SidePanel/side_panel_controller.h"

class AnimationsSidePanelView;

class AnimationsController : public SidePanelController
{
public:
	AnimationsController();

private:
	void update_animations();
	void update_animation_fields();

	void animations_list_selection_changed();
	void animations_list_edit_saved();
	void start_property_value_changed();
	void end_property_value_changed();
	void play_property_value_changed();
	void move_property_value_changed();
	void loop_property_value_changed();
	void rarity_property_value_changed();
	void fbx_filename_property_browse();

	std::shared_ptr<AnimationsSidePanelView> panel;

	uicore::SlotContainer slots;
};
