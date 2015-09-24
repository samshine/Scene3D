
#include "precomp.h"
#include "path_nodes_controller.h"
#include "Views/Rollout/rollout_view.h"
#include "Views/Rollout/rollout_list.h"
#include "Views/Rollout/rollout_text_field_property.h"
#include "Model/MapEditor/map_app_model.h"

using namespace uicore;

PathNodesController::PathNodesController()
{
	path_nodes = std::make_shared<RolloutView>("PATH NODES");
	path_node = std::make_shared<RolloutView>("PATH NODE");

	view->content_view()->add_subview(path_nodes);
	view->content_view()->add_subview(path_node);

	path_nodes_list = std::make_shared<RolloutList>();
	path_nodes_list->set_allow_edit(false);

	path_nodes->content->add_subview(path_nodes_list);

	slots.connect(path_nodes_list->sig_selection_changed(), this, &PathNodesController::path_nodes_list_selection_changed);
	slots.connect(path_nodes_list->sig_selection_clicked(), this, &PathNodesController::path_nodes_list_selection_clicked);

	slots.connect(MapAppModel::instance()->sig_load_finished, [this]() { update_path_nodes(); });

	update_path_nodes();
}

void PathNodesController::update_path_nodes()
{
	path_nodes_list->clear();
	bool first = true;

	std::map<std::string, std::shared_ptr<RolloutListItemView>> items;

	/*
	if (MapAppModel::instance()->fbx)
	{
		for (const auto &path_node_name : MapAppModel::instance()->fbx->path_node_names())
		{
			auto item = path_nodes_list->add_item(path_node_name);
			if (first)
			{
				item->set_selected(true, false);
				first = false;
			}
			items[path_node_name] = item;
		}
	}
	*/
	/*
	for (const auto &path_node : MapAppModel::instance()->desc.path_nodes)
	{
		auto &item = items[path_node.mesh_path_node];
		if (!item)
			item = path_nodes_list->add_item(path_node.mesh_path_node);

		item->set_bold(true);

		if (first)
		{
			item->set_selected(true, false);
			first = false;
		}
	}
	*/

	if (!path_nodes_list->selection())
		path_node->set_hidden(true);
}

int PathNodesController::get_select_item_index()
{
	/*auto selection = path_nodes_list->selection();
	if (selection)
	{
		std::string name = selection->text();

		const auto &path_nodes = MapAppModel::instance()->desc.path_nodes;
		for (size_t i = 0; i < path_nodes.size(); i++)
		{
			if (path_nodes[i].mesh_path_node == name)
				return (int)i;
		}
	}*/
	return -1;
}

void PathNodesController::update_path_node_fields()
{
	int index = get_select_item_index();

	if (index >= 0)
	{
		path_node->set_hidden(false);

		//const auto &path_node = MapAppModel::instance()->desc.path_nodes[index];
	}
	else
	{
		path_node->set_hidden(true);
	}
}

void PathNodesController::path_nodes_list_selection_changed()
{
	update_path_node_fields();
}

void PathNodesController::path_nodes_list_selection_clicked()
{
	auto selection = path_nodes_list->selection();
	if (selection)
	{
		/*
		if (selection->index >= MapAppModel::instance()->desc.path_nodes.size())
		{
			MapAppModel::instance()->desc.path_nodes.resize(selection->index + 1);
			path_nodes_list->add_item("");
		}

		auto &path_node = MapAppModel::instance()->desc.path_nodes[selection->index];
		path_node.name = selection->text();

		update_path_node_fields();
		*/
	}
}
