
#include "precomp.h"
#include "asset_explorer_controller.h"
#include "Model/AssetFolder/asset_folder_model.h"

using namespace uicore;

AssetExplorerController::AssetExplorerController()
{
	slots.connect(AssetFolderModel::instance()->sig_open_finished, this, &AssetExplorerController::populate_list_view);
	populate_list_view();
}

void AssetExplorerController::populate_list_view()
{
	auto model = AssetFolderModel::instance();

	auto root = view->tree_view->root_item();
	while (root->last_child())
		root->last_child()->remove_from_parent();

	if (model->project_folder.empty())
		return;

	add_items(root, model->assets_folder);
}

void AssetExplorerController::add_items(TreeItemPtr parent, const std::string &path)
{
	for (auto folder : Directory::directories(path))
	{
		auto item = parent->add_child(std::make_shared<TreeItem>(FilePath::filename(folder)));
		add_items(item, folder);
	}

	for (auto file : Directory::files(path))
	{
		parent->add_child(std::make_shared<TreeItem>(FilePath::filename(file)));
	}
}
