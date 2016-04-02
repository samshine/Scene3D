
#pragma once

#include "map_select_rollout.h"
#include "map_create_rollout.h"
#include "map_object_rollout.h"
#include "map_object_list_rollout.h"

class MapEditSidePanelView : public uicore::ColumnView
{
public:
	MapEditSidePanelView()
	{
		select = add_child<MapSelectRolloutView>();
		create = add_child<MapCreateRolloutView>();
		create_object = add_child<MapObjectRolloutView>();
		objects = add_child<MapObjectListRolloutView>();
		edit_object = add_child<MapObjectRolloutView>();
	}

	std::shared_ptr<MapSelectRolloutView> select;
	std::shared_ptr<MapCreateRolloutView> create;
	std::shared_ptr<MapObjectRolloutView> create_object;
	std::shared_ptr<MapObjectListRolloutView> objects;
	std::shared_ptr<MapObjectRolloutView> edit_object;
};
