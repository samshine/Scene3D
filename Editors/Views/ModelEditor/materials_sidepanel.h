
#pragma once

#include "Views/Rollout/rollout_view.h"
#include "Views/Rollout/rollout_list.h"
#include "Views/Rollout/rollout_text_field_property.h"

class MaterialsSidePanelView : public uicore::ColumnView
{
public:
	MaterialsSidePanelView()
	{
		materials = add_child<RolloutView>("MATERIALS");
		materials_list = materials->content->add_child<RolloutList>();
		materials_list->set_allow_edit(false);

		material = add_child<RolloutView>("MATERIAL");
		two_sided_property = material->content->add_child<RolloutTextFieldProperty>("TWO SIDED");
		alpha_test_property = material->content->add_child<RolloutTextFieldProperty>("ALPHA TEST");
		transparent_property = material->content->add_child<RolloutTextFieldProperty>("TRANSPARENT");
	}

	std::shared_ptr<RolloutView> materials;
	std::shared_ptr<RolloutList> materials_list;

	std::shared_ptr<RolloutView> material;
	std::shared_ptr<RolloutTextFieldProperty> two_sided_property;
	std::shared_ptr<RolloutTextFieldProperty> alpha_test_property;
	std::shared_ptr<RolloutTextFieldProperty> transparent_property;
};
