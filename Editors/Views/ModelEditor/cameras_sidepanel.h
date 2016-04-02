
#pragma once

#include "Views/Rollout/rollout_view.h"
#include "Views/Rollout/rollout_list.h"
#include "Views/Rollout/rollout_text_field_property.h"

class RolloutView;
class RolloutTextFieldProperty;
class RolloutBrowseFieldProperty;
class RolloutList;
class RolloutListItemView;

class CamerasSidePanelView : public uicore::ColumnView
{
public:
	CamerasSidePanelView()
	{
		cameras = add_child<RolloutView>("CAMERAS");
		cameras_list = cameras->content->add_child<RolloutList>();
		cameras_list->set_allow_edit(false);

		camera = add_child<RolloutView>("CAMERA");
	}

	std::shared_ptr<RolloutView> cameras;
	std::shared_ptr<RolloutList> cameras_list;

	std::shared_ptr<RolloutView> camera;
};
