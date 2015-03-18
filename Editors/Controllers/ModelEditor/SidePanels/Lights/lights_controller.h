
#pragma once

class RolloutView;
class RolloutTextFieldProperty;
class RolloutList;
class RolloutListItemView;

class LightsController : public clan::ViewController
{
public:
	LightsController();

private:
	void update_lights();
	void update_light_fields();

	void lights_list_selection_changed();
	void lights_list_selection_clicked();

	int get_select_item_index();

	std::shared_ptr<RolloutView> lights;
	std::shared_ptr<RolloutList> lights_list;

	std::shared_ptr<RolloutView> light;

	clan::SlotContainer slots;
};