
#pragma once

#include "Controllers/SidePanel/side_panel_controller.h"

class RolloutView;
class RolloutTextFieldProperty;
class RolloutList;
class RolloutListItemView;

class MapEmittersController : public SidePanelController
{
public:
	MapEmittersController();

private:
	void update_emitters();
	void update_emitter_fields();

	void emitters_list_selection_changed();
	void emitters_list_selection_clicked();

	int get_select_item_index();

	std::shared_ptr<RolloutView> emitters;
	std::shared_ptr<RolloutList> emitters_list;

	std::shared_ptr<RolloutView> emitter;

	clan::SlotContainer slots;
};
