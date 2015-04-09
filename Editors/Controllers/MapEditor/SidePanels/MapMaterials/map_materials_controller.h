
#pragma once

#include "Controllers/SidePanel/side_panel_controller.h"

class RolloutView;
class RolloutTextFieldProperty;
class RolloutList;
class RolloutListItemView;

class MapMaterialsController : public SidePanelController
{
public:
	MapMaterialsController();

private:
	void update_materials();
	void update_material_fields();

	void materials_list_selection_changed();
	void materials_list_selection_clicked();

	int get_select_item_index();

	std::shared_ptr<RolloutView> materials;
	std::shared_ptr<RolloutList> materials_list;

	std::shared_ptr<RolloutView> material;

	clan::SlotContainer slots;
};
