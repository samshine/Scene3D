
#pragma once

#include "Controllers/SidePanel/side_panel_controller.h"

class RolloutView;
class RolloutPositionProperty;
class RolloutTextFieldProperty;
class RolloutBrowseFieldProperty;
class RolloutList;
class RolloutListItemView;

class ObjectsController : public SidePanelController
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
	std::shared_ptr<RolloutTextFieldProperty> id;
	std::shared_ptr<RolloutPositionProperty> position;
	std::shared_ptr<RolloutTextFieldProperty> dir;
	std::shared_ptr<RolloutTextFieldProperty> up;
	std::shared_ptr<RolloutTextFieldProperty> tilt;
	std::shared_ptr<RolloutTextFieldProperty> scale;
	std::shared_ptr<RolloutBrowseFieldProperty> model_desc_filename;

	uicore::SlotContainer slots;
};
