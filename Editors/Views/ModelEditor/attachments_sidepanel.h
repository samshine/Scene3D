
#pragma once

#include "Views/Rollout/rollout_view.h"
#include "Views/Rollout/rollout_list.h"
#include "Views/Rollout/rollout_position_property.h"
#include "Views/Rollout/rollout_text_field_property.h"
#include "Views/Rollout/rollout_browse_field_property.h"

class AttachmentsSidePanelView : public uicore::ColumnView
{
public:
	AttachmentsSidePanelView()
	{
		attachments = add_child<RolloutView>("ATTACHMENTS");
		attachments_list = attachments->content->add_child<RolloutList>();

		attachment = add_child<RolloutView>("ATTACHMENT");
		position_property = attachment->content->add_child<RolloutPositionProperty>("POSITION");
		orientation_property = attachment->content->add_child<RolloutPositionProperty>("ORIENTATION");
		bone_name_property = attachment->content->add_child<RolloutTextFieldProperty>("BONE NAME");
		test_model_property = attachment->content->add_child<RolloutBrowseFieldProperty>("TEST MODEL");
		test_scale_property = attachment->content->add_child<RolloutTextFieldProperty>("TEST SCALE");
	}

	std::shared_ptr<RolloutView> attachments;
	std::shared_ptr<RolloutList> attachments_list;

	std::shared_ptr<RolloutView> attachment;
	std::shared_ptr<RolloutPositionProperty> position_property;
	std::shared_ptr<RolloutPositionProperty> orientation_property;
	std::shared_ptr<RolloutTextFieldProperty> bone_name_property;
	std::shared_ptr<RolloutBrowseFieldProperty> test_model_property;
	std::shared_ptr<RolloutTextFieldProperty> test_scale_property;
};
