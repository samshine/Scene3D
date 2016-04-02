
#pragma once

#include "Controllers/SidePanel/side_panel_controller.h"

class EnvironmentSidePanelView;

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

	std::shared_ptr<EnvironmentSidePanelView> panel;

	uicore::SlotContainer slots;
};
