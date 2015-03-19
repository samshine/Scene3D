
#include "precomp.h"
#include "map_editor_window.h"
#include "Model/MapEditor/map_app_model.h"
#include "Views/Header/header_view.h"
#include "Views/Header/header_menu_view.h"
#include "Controllers/Workspace/workspace_controller.h"
#include "Controllers/MapEditor/MapScene/map_scene_controller.h"
#include "Controllers/MapEditor/SidePanels/Objects/objects_controller.h"

using namespace clan;

MapEditorWindow::MapEditorWindow()
{
	create_layout();

	slots.connect(view->sig_close(), [this](CloseEvent &e)
	{
		RunLoop::exit();
	});

	auto app_menu = header_view->add_left_menu("Editor", "Icons/App/AppIcon-32.png");

	app_menu->add_item("Open", bind_member(this, &MapEditorWindow::on_open));
	app_menu->add_item("Save", bind_member(this, &MapEditorWindow::on_save));
	app_menu->add_item("Save As", bind_member(this, &MapEditorWindow::on_save_as));

	header_view->add_right_button("Undo", "Icons/Undo/undo-24.png", []() {});
	header_view->add_right_button("Redo", "Icons/Redo/redo-24.png", []() {});

	header_view->add_right_button("Change Model", "Icons/Model/model-24.png", bind_member(this, &MapEditorWindow::on_change_model));

	auto options = header_view->add_right_menu("Options", "Icons/Menu/menu-24.png", true);
	options->add_item("Objects", bind_member(this, &MapEditorWindow::on_show_objects));

	scene_controller = std::make_shared<MapSceneController>();
	objects_controller = std::make_shared<ObjectsController>();

	workspace_controller->set_center(scene_controller);
	workspace_controller->set_docked(objects_controller);
}

void MapEditorWindow::create_layout()
{
	DisplayWindowDescription desc;
	desc.set_size(Size(1280, 768), false);
	desc.set_title("Scene3D Map Editor");
	desc.set_allow_resize(true);
	view = std::make_shared<WindowView>(desc);

	window_view()->get_display_window().set_large_icon(ImageProviderFactory::load("Resources/Icons/App/AppIcon-256.png"));
	window_view()->get_display_window().set_small_icon(ImageProviderFactory::load("Resources/Icons/App/AppIcon-16.png"));

	view->style()->set("flex-direction: column");
	view->style()->set("background: rgb(240,240,240)");

	header_view = std::make_shared<HeaderView>();
	workspace_controller = std::make_shared<WorkspaceController>();

	view->add_subview(header_view);
	add_child_controller(workspace_controller);
}

void MapEditorWindow::update_window_title()
{
	if (!MapAppModel::instance()->open_filename.empty())
		window_view()->get_display_window().set_title(PathHelp::get_basename(MapAppModel::instance()->open_filename) + " - Scene3D Map Editor");
	else
		window_view()->get_display_window().set_title("Model Editor");
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
		MapAppModel::instance()->open(dialog.get_filename());
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
		std::string filename = dialog.get_filename();
		if (PathHelp::get_extension(filename).empty())
			filename += ".mapdesc";
		MapAppModel::instance()->save(filename);
		update_window_title();
	}
}

void MapEditorWindow::on_change_model()
{
	OpenFileDialog dialog(view.get());
	dialog.set_title("Select Model");
	dialog.add_filter("Autodesk FBX files (*.fbx)", "*.fbx", true);
	dialog.add_filter("All files (*.*)", "*.*", false);
	dialog.set_filename(MapAppModel::instance()->desc.fbx_filename);
	if (dialog.show())
	{
		MapAppModel::instance()->set_fbx_model(dialog.get_filename());
	}
}

void MapEditorWindow::on_show_objects()
{
	workspace_controller->set_docked(objects_controller);
}
