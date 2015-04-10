
#pragma once

#include "Controllers/SidePanel/side_panel_controller.h"

class RolloutView;
class RolloutPositionProperty;
class RolloutTextFieldProperty;
class RolloutList;
class RolloutListItemView;

class MapLightProbesController : public SidePanelController
{
public:
	MapLightProbesController();

private:
	void update_light_probes();
	void update_light_probe_fields();

	void light_probes_list_selection_changed();
	void light_probes_list_selection_clicked();

	int get_select_item_index();

	std::shared_ptr<RolloutView> light_probes;
	std::shared_ptr<RolloutList> light_probes_list;

	std::shared_ptr<RolloutView> light_probe;
	std::shared_ptr<RolloutPositionProperty> position;

	clan::SlotContainer slots;
};
