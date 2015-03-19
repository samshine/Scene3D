
#pragma once

class RolloutView;
class RolloutTextFieldProperty;
class RolloutList;
class RolloutListItemView;

class ObjectsController : public clan::ViewController
{
public:
	ObjectsController();

private:
	void update_objects();
	void update_object_fields();

	void objects_list_selection_changed();
	void objects_list_selection_clicked();

	int get_select_item_index();

	std::shared_ptr<RolloutView> objects;
	std::shared_ptr<RolloutList> objects_list;

	std::shared_ptr<RolloutView> object;

	clan::SlotContainer slots;
};
