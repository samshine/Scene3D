
#pragma once

#include "Controllers/SidePanel/side_panel_controller.h"

class RolloutView;
class RolloutTextFieldProperty;
class RolloutBrowseFieldProperty;

class EnvironmentController : public SidePanelController
{
public:
	EnvironmentController();

private:
	void map_model_updated();

	void map_model_property_browse();
	void gravity_property_value_changed();
	void height_property_value_changed();
	void radius_property_value_changed();
	void step_height_property_value_changed();
	void mass_property_value_changed();
	void acceleration_property_value_changed();
	void run_speed_property_value_changed();
	void friction_property_value_changed();
	void air_resistance_property_value_changed();
	void air_movement_property_value_changed();
	void bounce_property_value_changed();

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

	clan::SlotContainer slots;
};
