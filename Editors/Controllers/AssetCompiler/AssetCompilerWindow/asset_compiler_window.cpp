
#include "precomp.h"
#include "asset_compiler_window.h"
#include "Views/Header/header_menu_view.h"
#include "Model/AssetFolder/asset_folder_model.h"

using namespace uicore;

AssetCompilerWindow::AssetCompilerWindow()
{
	set_title("Scene3D Asset Editor");
	set_frame_size({ 1200.0f, 700.0f });
	set_icon({ "Icons/App/AppIcon-256.png", "Icons/App/AppIcon-16.png" });
	set_root_view(view);

	slots.connect(view->sig_close(), [this](CloseEvent *)
	{
		dismiss();
	});

	view->sidebar->add_child(asset_explorer_controller->view);
	view->center->add_child(build_controller->view);

	auto app_menu = view->header->add_left_menu("Asset Editor", "Icons/App/AppIcon-32.png");

	app_menu->add_item("Open", bind_member(this, &AssetCompilerWindow::on_open));
}

void AssetCompilerWindow::on_open()
{
	BrowseFolderDialog dialog(view.get());
	dialog.set_title("Select Asset Folder");
	dialog.set_initial_directory(AssetFolderModel::instance()->project_folder);
	if (dialog.show())
	{
		AssetFolderModel::instance()->open(dialog.selected_path());
		update_window_title();
	}
}

void AssetCompilerWindow::update_window_title()
{
	if (!AssetFolderModel::instance()->project_folder.empty())
		set_title(FilePath::filename_without_extension(AssetFolderModel::instance()->project_folder) + " - Scene3D Asset Editor");
	else
		set_title("Scene3D Asset Editor");
}
