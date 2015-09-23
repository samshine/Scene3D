
#pragma once

#include "Controllers/SidePanel/side_panel_controller.h"

class RolloutView;
class RolloutPositionProperty;
class RolloutTextFieldProperty;
class RolloutBrowseFieldProperty;
class RolloutList;
class RolloutListItemView;

class AttachmentsController : public SidePanelController
{
public:
	AttachmentsController();

private:
	void update_attachments();
	void update_attachment_fields();

	void attachments_list_selection_changed();
	void attachments_list_edit_saved();
	void position_property_value_changed();
	void orientation_property_value_changed();
	void bone_name_property_value_changed();
	void test_model_property_browse();
	void test_scale_property_value_changed();

	std::shared_ptr<RolloutView> attachments;
	std::shared_ptr<RolloutList> attachments_list;

	std::shared_ptr<RolloutView> attachment;
	std::shared_ptr<RolloutPositionProperty> position_property;
	std::shared_ptr<RolloutPositionProperty> orientation_property;
	std::shared_ptr<RolloutTextFieldProperty> bone_name_property;
	std::shared_ptr<RolloutBrowseFieldProperty> test_model_property;
	std::shared_ptr<RolloutTextFieldProperty> test_scale_property;

	uicore::SlotContainer slots;
};
