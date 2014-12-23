
#pragma once

class RolloutView;
class RolloutTextFieldProperty;
class RolloutList;
class RolloutListItemView;

class MaterialsController : public clan::ViewController
{
public:
	MaterialsController();

private:
	void update_materials();
	void update_material_fields();

	void materials_list_selection_changed();
	void materials_list_edit_saved();
	void two_sided_property_value_changed();
	void alpha_test_property_value_changed();
	void mesh_material_property_value_changed();

	std::shared_ptr<RolloutView> materials;
	std::shared_ptr<RolloutList> materials_list;

	std::shared_ptr<RolloutView> material;
	std::shared_ptr<RolloutTextFieldProperty> two_sided_property;
	std::shared_ptr<RolloutTextFieldProperty> alpha_test_property;
	std::shared_ptr<RolloutTextFieldProperty> mesh_material_property;

	clan::SlotContainer slots;
};
