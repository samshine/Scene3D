
#pragma once

#include "Controllers/SidePanel/side_panel_controller.h"

class AttachmentsSidePanelView;

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

	std::shared_ptr<AttachmentsSidePanelView> panel;

	uicore::SlotContainer slots;
};
