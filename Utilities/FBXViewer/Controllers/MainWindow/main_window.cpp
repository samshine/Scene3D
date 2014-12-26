
#include "precomp.h"
#include "main_window.h"
#include "Model/app_model.h"
#include "Views/Header/header_view.h"
#include "Controllers/Workspace/workspace_controller.h"
#include "Controllers/Scene/scene_controller.h"
#include "Controllers/Animations/animations_controller.h"
#include "Controllers/Attachments/attachments_controller.h"
#include "Controllers/Materials/materials_controller.h"
#include "Controllers/Lights/lights_controller.h"
#include "Controllers/Bones/bones_controller.h"
#include "Controllers/Cameras/cameras_controller.h"

using namespace clan;

MainWindow::MainWindow()
{
	create_layout();

	slots.connect(view->sig_close(), [this](CloseEvent &e)
	{
		exit = true;
	});

	header_view->add_left_button("Load", bind_member(this, &MainWindow::on_load));
	header_view->add_left_button("Save", bind_member(this, &MainWindow::on_save));
	header_view->add_left_button("Save As", bind_member(this, &MainWindow::on_save_as));

	header_view->add_right_button("Change Model", bind_member(this, &MainWindow::on_change_model));
	header_view->add_right_button("Attachments", bind_member(this, &MainWindow::on_show_attachments));
	header_view->add_right_button("Animations", bind_member(this, &MainWindow::on_show_animations));
	header_view->add_right_button("Materials", bind_member(this, &MainWindow::on_show_materials));
	header_view->add_right_button("Lights", bind_member(this, &MainWindow::on_show_lights));
	header_view->add_right_button("Bones", bind_member(this, &MainWindow::on_show_bones));
	header_view->add_right_button("Cameras", bind_member(this, &MainWindow::on_show_cameras));

	scene_controller = std::make_shared<SceneController>();
	animations_controller = std::make_shared<AnimationsController>();
	attachments_controller = std::make_shared<AttachmentsController>();
	materials_controller = std::make_shared<MaterialsController>();
	lights_controller = std::make_shared<LightsController>();
	bones_controller = std::make_shared<BonesController>();
	cameras_controller = std::make_shared<CamerasController>();

	workspace_controller->set_center(scene_controller);
	workspace_controller->set_docked(animations_controller);
}

void MainWindow::create_layout()
{
	DisplayWindowDescription desc;
	desc.set_size(Size(1280, 768), false);
	desc.set_title("FBX Viewer");
	desc.set_allow_resize(true);
	view = std::make_shared<WindowView>(desc);

	view->box_style.set_layout_vbox();
	view->box_style.set_background(Colorf(240, 240, 240));

	header_view = std::make_shared<HeaderView>();
	workspace_controller = std::make_shared<WorkspaceController>();

	view->add_subview(header_view);
	add_child_controller(workspace_controller);
}

void MainWindow::on_load()
{
	OpenFileDialog dialog(view.get());
	dialog.set_title("Select Model");
	dialog.add_filter("Model description files (*.modeldesc)", "*.modeldesc", true);
	dialog.add_filter("All files (*.*)", "*.*", false);
	dialog.set_filename(AppModel::instance()->open_filename);
	if (dialog.show())
	{
		AppModel::instance()->open(dialog.get_filename());
	}
}

void MainWindow::on_save()
{
	if (AppModel::instance()->open_filename.empty())
	{
		on_save_as();
	}
	else
	{
		AppModel::instance()->save(AppModel::instance()->open_filename);
	}
}

void MainWindow::on_save_as()
{
	SaveFileDialog dialog(view.get());
	dialog.set_title("Save Model Description");
	dialog.add_filter("Model description files (*.modeldesc)", "*.modeldesc", true);
	dialog.add_filter("All files (*.*)", "*.*", false);
	dialog.set_filename(AppModel::instance()->open_filename);
	if (dialog.show())
	{
		std::string filename = dialog.get_filename();
		if (PathHelp::get_extension(filename).empty())
			filename += ".modeldesc";
		AppModel::instance()->save(filename);
	}
}

void MainWindow::on_change_model()
{
	OpenFileDialog dialog(view.get());
	dialog.set_title("Select Model");
	dialog.add_filter("Autodesk FBX files (*.fbx)", "*.fbx", true);
	dialog.add_filter("All files (*.*)", "*.*", false);
	dialog.set_filename(AppModel::instance()->desc.fbx_filename);
	if (dialog.show())
	{
		AppModel::instance()->set_fbx_model(dialog.get_filename());
	}
}

void MainWindow::on_show_attachments()
{
	workspace_controller->set_docked(attachments_controller);
}

void MainWindow::on_show_animations()
{
	workspace_controller->set_docked(animations_controller);
}

void MainWindow::on_show_materials()
{
	workspace_controller->set_docked(materials_controller);
}

void MainWindow::on_show_lights()
{
	workspace_controller->set_docked(lights_controller);
}

void MainWindow::on_show_bones()
{
	workspace_controller->set_docked(bones_controller);
}

void MainWindow::on_show_cameras()
{
	workspace_controller->set_docked(cameras_controller);
}
