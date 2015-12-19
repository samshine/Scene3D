
#pragma once

#include "Views/Rollout/rollout_view.h"
#include "Views/Rollout/rollout_list.h"
#include <memory>

class MapObjectListRolloutView : public RolloutView
{
public:
	MapObjectListRolloutView() : RolloutView("OBJECTS")
	{
		objects_list = content->add_subview<RolloutList>();
		objects_list->set_allow_edit(false);
	}

	std::shared_ptr<RolloutList> objects_list;
};
