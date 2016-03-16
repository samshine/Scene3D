
#pragma once

#include "Views/Rollout/rollout_view.h"
#include "Views/Rollout/rollout_button.h"
#include <memory>

class MapSelectRolloutView : public RolloutView
{
public:
	MapSelectRolloutView() : RolloutView("SELECT")
	{
		button_select = content->add_child<RolloutButton>("SELECT");
		button_select_move = content->add_child<RolloutButton>("MOVE");
		button_select_rotate = content->add_child<RolloutButton>("ROTATE");
		button_select_scale = content->add_child<RolloutButton>("SCALE");
	}

	std::shared_ptr<RolloutButton> button_select;
	std::shared_ptr<RolloutButton> button_select_move;
	std::shared_ptr<RolloutButton> button_select_rotate;
	std::shared_ptr<RolloutButton> button_select_scale;
};
