
#pragma once

#include "Views/Rollout/rollout_view.h"
#include "Views/Rollout/rollout_text_field_property.h"
#include "Views/Rollout/rollout_browse_field_property.h"

class EnvironmentSidePanelView : public uicore::ColumnView
{
public:
	EnvironmentSidePanelView()
	{
		environment = add_child<RolloutView>("ENVIRONMENT");
		map_model_property = environment->content->add_child<RolloutBrowseFieldProperty>("MAP MODEL");

		character = add_child<RolloutView>("CHARACTER CONTROLLER");
		gravity_property = character->content->add_child<RolloutTextFieldProperty>("GRAVITY");
		height_property = character->content->add_child<RolloutTextFieldProperty>("HEIGHT");
		radius_property = character->content->add_child<RolloutTextFieldProperty>("RADIUS");
		step_height_property = character->content->add_child<RolloutTextFieldProperty>("STEP HEIGHT");
		mass_property = character->content->add_child<RolloutTextFieldProperty>("MASS");
		acceleration_property = character->content->add_child<RolloutTextFieldProperty>("ACCELERATION");
		run_speed_property = character->content->add_child<RolloutTextFieldProperty>("RUN SPEED");
		friction_property = character->content->add_child<RolloutTextFieldProperty>("FRICTION");
		air_resistance_property = character->content->add_child<RolloutTextFieldProperty>("AIR RESISTANCE");
		air_movement_property = character->content->add_child<RolloutTextFieldProperty>("AIR MOVEMENT");
		bounce_property = character->content->add_child<RolloutTextFieldProperty>("BOUNCE");
	}

	std::shared_ptr<RolloutView> environment;
	std::shared_ptr<RolloutBrowseFieldProperty> map_model_property;

	std::shared_ptr<RolloutView> character;
	std::shared_ptr<RolloutTextFieldProperty> gravity_property;
	std::shared_ptr<RolloutTextFieldProperty> height_property;
	std::shared_ptr<RolloutTextFieldProperty> radius_property;
	std::shared_ptr<RolloutTextFieldProperty> step_height_property;
	std::shared_ptr<RolloutTextFieldProperty> mass_property;
	std::shared_ptr<RolloutTextFieldProperty> acceleration_property;
	std::shared_ptr<RolloutTextFieldProperty> run_speed_property;
	std::shared_ptr<RolloutTextFieldProperty> friction_property;
	std::shared_ptr<RolloutTextFieldProperty> air_resistance_property;
	std::shared_ptr<RolloutTextFieldProperty> air_movement_property;
	std::shared_ptr<RolloutTextFieldProperty> bounce_property;
};
