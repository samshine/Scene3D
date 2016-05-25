
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

	auto assets = root->add_child(std::make_shared<TreeItem>("Assets"));

	auto content = assets->add_child(std::make_shared<TreeItem>("Content"));
	auto files = assets->add_child(std::make_shared<TreeItem>("Files"));
	auto templates = assets->add_child(std::make_shared<TreeItem>("Templates"));

	auto config = content->add_child(std::make_shared<TreeItem>("Config"));
	auto levels = content->add_child(std::make_shared<TreeItem>("Levels"));
	auto models = content->add_child(std::make_shared<TreeItem>("Models"));

	config->add_child(std::make_shared<TreeItem>("Game"));
	config->add_child(std::make_shared<TreeItem>("Input"));

	levels->add_child(std::make_shared<TreeItem>("Liandri"));
	levels->add_child(std::make_shared<TreeItem>("Temple"));

	auto character = models->add_child(std::make_shared<TreeItem>("Character"));
	auto door = models->add_child(std::make_shared<TreeItem>("Door"));
	auto lift = models->add_child(std::make_shared<TreeItem>("Lift"));
	auto weapon = models->add_child(std::make_shared<TreeItem>("Weapon"));

	character->add_child(std::make_shared<TreeItem>("Kachujin"));
	character->add_child(std::make_shared<TreeItem>("Medea"));
	character->add_child(std::make_shared<TreeItem>("XBot"));
	character->add_child(std::make_shared<TreeItem>("YBot"));
	character->add_child(std::make_shared<TreeItem>("Zombie"));

	weapon->add_child(std::make_shared<TreeItem>("Shotgun"));
	weapon->add_child(std::make_shared<TreeItem>("Rocket Launcher"));

	templates->add_child(std::make_shared<TreeItem>("Folder"));
	templates->add_child(std::make_shared<TreeItem>("Level"));
	templates->add_child(std::make_shared<TreeItem>("Model"));
	templates->add_child(std::make_shared<TreeItem>("Weapon"));
	templates->add_child(std::make_shared<TreeItem>("Settings"));

	add_items(files, model->assets_folder);
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
