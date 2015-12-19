
#pragma once

#include "Views/Rollout/rollout_view.h"
#include "Views/Rollout/rollout_button.h"
#include <memory>

class MapSelectRolloutView : public RolloutView
{
public:
	MapSelectRolloutView() : RolloutView("SELECT")
	{
		button_select = content->add_subview<RolloutButton>("SELECT");
		button_select_move = content->add_subview<RolloutButton>("MOVE");
		button_select_rotate = content->add_subview<RolloutButton>("ROTATE");
		button_select_scale = content->add_subview<RolloutButton>("SCALE");
	}

	std::shared_ptr<RolloutButton> button_select;
	std::shared_ptr<RolloutButton> button_select_move;
	std::shared_ptr<RolloutButton> button_select_rotate;
	std::shared_ptr<RolloutButton> button_select_scale;
};
