
#pragma once

#include "Views/Rollout/rollout_view.h"
#include "Views/Rollout/rollout_button.h"
#include <memory>

class MapCreateRolloutView : public RolloutView
{
public:
	MapCreateRolloutView() : RolloutView("CREATE")
	{
		button_create_object = content->add_subview<RolloutButton>("OBJECT");
		button_create_trigger = content->add_subview<RolloutButton>("TRIGGER");
		button_create_path_point = content->add_subview<RolloutButton>("PATH POINT");
		button_create_light_probe = content->add_subview<RolloutButton>("LIGHT PROBE");
	}

	std::shared_ptr<RolloutButton> button_create_object;
	std::shared_ptr<RolloutButton> button_create_trigger;
	std::shared_ptr<RolloutButton> button_create_path_point;
	std::shared_ptr<RolloutButton> button_create_light_probe;
};
