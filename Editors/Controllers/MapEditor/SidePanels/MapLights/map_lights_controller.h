
#pragma once

#include "Controllers/SidePanel/side_panel_controller.h"

class RolloutView;
class RolloutPositionProperty;
class RolloutTextFieldProperty;
class RolloutList;
class RolloutListItemView;

class MapLightsController : public SidePanelController
{
public:
	MapLightsController();

private:
	void update_lights();
	void update_light_fields();

	void lights_list_selection_changed();
	void lights_list_selection_clicked();

	int get_select_item_index();

	std::shared_ptr<RolloutView> lights;
	std::shared_ptr<RolloutList> lights_list;

	std::shared_ptr<RolloutView> light;
	std::shared_ptr<RolloutTextFieldProperty> mesh_light;
	std::shared_ptr<RolloutTextFieldProperty> bake;

	clan::SlotContainer slots;
};
