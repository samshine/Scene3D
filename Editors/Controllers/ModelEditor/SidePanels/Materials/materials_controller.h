
#pragma once

#include "Controllers/SidePanel/side_panel_controller.h"

class MaterialsSidePanelView;

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

	std::shared_ptr<MaterialsSidePanelView> panel;

	uicore::SlotContainer slots;
};
