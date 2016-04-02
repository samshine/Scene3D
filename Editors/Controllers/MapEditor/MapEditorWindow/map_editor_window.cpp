
#include "precomp.h"
#include "map_editor_window.h"
#include "Model/MapEditor/map_app_model.h"
#include "Views/Header/header_view.h"
#include "Views/Header/header_menu_view.h"
#include "Controllers/Workspace/workspace_controller.h"
#include "Controllers/MapEditor/MapScene/map_scene_controller.h"
#include "Controllers/MapEditor/SidePanels/Edit/map_edit_controller.h"

using namespace uicore;

MapEditorWindow::MapEditorWindow()
{
	set_root_view(view);
	set_title("Scene3D Map Editor");
	set_frame_size({ 1280.0f, 768.0f });
	set_icon({"Icons/App/AppIcon-256.png", "Icons/App/AppIcon-16.png"});

	view->style()->set("background: rgb(240,240,240)");

	header_view = std::make_shared<HeaderView>();
	workspace_controller = std::make_shared<WorkspaceController>();

	view->add_child(header_view);
	view->add_child(workspace_controller->view);

	slots.connect(view->sig_close(), [this](CloseEvent *)
	{
		dismiss();
	});

	auto app_menu = header_view->add_left_menu("Map Editor", "Icons/App/AppIcon-32.png");

	app_menu->add_item("Open", bind_member(this, &MapEditorWindow::on_open));
	app_menu->add_item("Save", bind_member(this, &MapEditorWindow::on_save));
	app_menu->add_item("Save As", bind_member(this, &MapEditorWindow::on_save_as));

	header_view->add_right_button("Undo", "Icons/Undo/undo-24.png", []() {});
	header_view->add_right_button("Redo", "Icons/Redo/redo-24.png", []() {});

	auto options = header_view->add_right_menu("Options", "Icons/Menu/menu-24.png", true);
	options->add_item("Edit", bind_member(this, &MapEditorWindow::on_show_edit));

	scene_controller = std::make_shared<MapSceneController>();
	edit_controller = std::make_shared<MapEditController>();

	workspace_controller->set_center(scene_controller);
	workspace_controller->set_docked(edit_controller);
}

void MapEditorWindow::update_window_title()
{
	if (!MapAppModel::instance()->open_filename.empty())
		set_title(PathHelp::basename(MapAppModel::instance()->open_filename) + " - Scene3D Map Editor");
	else
		set_title("Model Editor");
}

void MapEditorWindow::on_open()
{
	OpenFileDialog dialog(view.get());
	dialog.set_title("Select Model");
	dialog.add_filter("Map description files (*.mapdesc)", "*.mapdesc", true);
	dialog.add_filter("All files (*.*)", "*.*", false);
	dialog.set_filename(MapAppModel::instance()->open_filename);
	if (dialog.show())
	{
		MapAppModel::instance()->open(dialog.filename());
		update_window_title();
	}
}

void MapEditorWindow::on_save()
{
	if (MapAppModel::instance()->open_filename.empty())
	{
		on_save_as();
	}
	else
	{
		MapAppModel::instance()->save(MapAppModel::instance()->open_filename);
	}
}

void MapEditorWindow::on_save_as()
{
	SaveFileDialog dialog(view.get());
	dialog.set_title("Save Model Description");
	dialog.add_filter("Map description files (*.mapdesc)", "*.mapdesc", true);
	dialog.add_filter("All files (*.*)", "*.*", false);
	dialog.set_filename(MapAppModel::instance()->open_filename);
	if (dialog.show())
	{
		std::string filename = dialog.filename();
		if (PathHelp::extension(filename).empty())
			filename += ".mapdesc";
		MapAppModel::instance()->save(filename);
		update_window_title();
	}
}

void MapEditorWindow::on_show_edit()
{
	workspace_controller->set_docked(edit_controller);
}
