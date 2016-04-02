
#pragma once

#include "Views/Rollout/rollout_view.h"
#include "Views/Rollout/rollout_list.h"
#include "Views/Rollout/rollout_text_field_property.h"
#include "Views/Rollout/rollout_position_property.h"

class BonesSidePanelView : public uicore::ColumnView
{
public:
	BonesSidePanelView()
	{
		bones = add_child<RolloutView>("BONES");
		bones_list = bones->content->add_child<RolloutList>();
		bones_list->set_allow_edit(false);

		bone_rollout = add_child<RolloutView>("BONE");
		name_property = bone_rollout->content->add_child<RolloutTextFieldProperty>("NAME");
		mesh_bone_property = bone_rollout->content->add_child<RolloutTextFieldProperty>("MESH BONE");
		shape_property = bone_rollout->content->add_child<RolloutTextFieldProperty>("SHAPE");
		position_property = bone_rollout->content->add_child<RolloutPositionProperty>("POSITION");
		rotation_property = bone_rollout->content->add_child<RolloutPositionProperty>("ROTATION");
		extents_property = bone_rollout->content->add_child<RolloutPositionProperty>("EXTENTS");
	}

	std::shared_ptr<RolloutView> bones;
	std::shared_ptr<RolloutList> bones_list;

	std::shared_ptr<RolloutView> bone_rollout;
	std::shared_ptr<RolloutTextFieldProperty> name_property;
	std::shared_ptr<RolloutTextFieldProperty> mesh_bone_property;
	std::shared_ptr<RolloutTextFieldProperty> shape_property;
	std::shared_ptr<RolloutPositionProperty> position_property;
	std::shared_ptr<RolloutPositionProperty> rotation_property;
	std::shared_ptr<RolloutPositionProperty> extents_property;
};
