
#pragma once

#include "Controllers/SidePanel/side_panel_controller.h"

class RolloutView;
class RolloutTextFieldProperty;
class RolloutBrowseFieldProperty;
class RolloutList;
class RolloutListItemView;

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

	std::shared_ptr<RolloutView> animations;
	std::shared_ptr<RolloutList> animations_list;

	std::shared_ptr<RolloutView> animation;
	std::shared_ptr<RolloutTextFieldProperty> start_property;
	std::shared_ptr<RolloutTextFieldProperty> end_property;
	std::shared_ptr<RolloutTextFieldProperty> play_property;
	std::shared_ptr<RolloutTextFieldProperty> move_property;
	std::shared_ptr<RolloutTextFieldProperty> loop_property;
	std::shared_ptr<RolloutTextFieldProperty> rarity_property;
	std::shared_ptr<RolloutBrowseFieldProperty> fbx_filename_property;

	uicore::SlotContainer slots;
};
