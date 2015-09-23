
#pragma once

#include "Controllers/SidePanel/side_panel_controller.h"

class RolloutView;
class RolloutTextFieldProperty;
class RolloutList;
class RolloutListItemView;

class PathNodesController : public SidePanelController
{
public:
	PathNodesController();

private:
	void update_path_nodes();
	void update_path_node_fields();

	void path_nodes_list_selection_changed();
	void path_nodes_list_selection_clicked();

	int get_select_item_index();

	std::shared_ptr<RolloutView> path_nodes;
	std::shared_ptr<RolloutList> path_nodes_list;

	std::shared_ptr<RolloutView> path_node;

	uicore::SlotContainer slots;
};
