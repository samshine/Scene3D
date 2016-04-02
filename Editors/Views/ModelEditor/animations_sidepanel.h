
#pragma once

#include "Views/Rollout/rollout_view.h"
#include "Views/Rollout/rollout_list.h"
#include "Views/Rollout/rollout_text_field_property.h"
#include "Views/Rollout/rollout_browse_field_property.h"

class AnimationsSidePanelView : public uicore::ColumnView
{
public:
	AnimationsSidePanelView()
	{
		animations = add_child<RolloutView>("ANIMATIONS");
		animations_list = animations->content->add_child<RolloutList>();

		animation = add_child<RolloutView>("ANIMATION");
		start_property = animation->content->add_child<RolloutTextFieldProperty>("START FRAME");
		end_property = animation->content->add_child<RolloutTextFieldProperty>("END FRAME");
		play_property = animation->content->add_child<RolloutTextFieldProperty>("PLAY SPEED");
		move_property = animation->content->add_child<RolloutTextFieldProperty>("MOVE SPEED");
		loop_property = animation->content->add_child<RolloutTextFieldProperty>("LOOP");
		rarity_property = animation->content->add_child<RolloutTextFieldProperty>("RARITY");
		fbx_filename_property = animation->content->add_child<RolloutBrowseFieldProperty>("BONES MODEL");
	}

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
};
