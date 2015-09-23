
#pragma once

#include "Controllers/SidePanel/side_panel_controller.h"

class RolloutView;
class RolloutTextFieldProperty;
class RolloutList;
class RolloutListItemView;

class MaterialsController : public SidePanelController
{
public:
	MaterialsController();

private:
	void update_materials();
	void update_material_fields();

	void materials_list_selection_changed();
	void materials_list_selection_clicked();
	void two_sided_property_value_changed();
	void alpha_test_property_value_changed();
	void transparent_property_value_changed();

	int get_select_item_index();

	std::shared_ptr<RolloutView> materials;
	std::shared_ptr<RolloutList> materials_list;

	std::shared_ptr<RolloutView> material;
	std::shared_ptr<RolloutTextFieldProperty> two_sided_property;
	std::shared_ptr<RolloutTextFieldProperty> alpha_test_property;
	std::shared_ptr<RolloutTextFieldProperty> transparent_property;

	uicore::SlotContainer slots;
};
