
#include "precomp.h"
#include "model_editor_window.h"
#include "Model/ModelEditor/model_app_model.h"
#include "Views/Header/header_view.h"
#include "Views/Header/header_menu_view.h"
#include "Controllers/Workspace/workspace_controller.h"
#include "Controllers/ModelEditor/Scene/scene_controller.h"
#include "Controllers/ModelEditor/SidePanels/Animations/animations_controller.h"
#include "Controllers/ModelEditor/SidePanels/Attachments/attachments_controller.h"
#include "Controllers/ModelEditor/SidePanels/Materials/materials_controller.h"
#include "Controllers/ModelEditor/SidePanels/Lights/lights_controller.h"
#include "Controllers/ModelEditor/SidePanels/Bones/bones_controller.h"
#include "Controllers/ModelEditor/SidePanels/Cameras/cameras_controller.h"
#include "Controllers/ModelEditor/SidePanels/Environment/environment_controller.h"

using namespace clan;

ModelEditorWindow::ModelEditorWindow()
{
	create_layout();

	slots.connect(view->sig_close(), [this](CloseEvent &e)
	{
		RunLoop::exit();
	});

	auto app_menu = header_view->add_left_menu("Model Editor", "Icons/App/AppIcon-32.png");

	app_menu->add_item("Open", bind_member(this, &ModelEditorWindow::on_open));
	app_menu->add_item("Save", bind_member(this, &ModelEditorWindow::on_save));
	app_menu->add_item("Save As", bind_member(this, &ModelEditorWindow::on_save_as));
	app_menu->add_item("Compile", bind_member(this, &ModelEditorWindow::on_compile));

	header_view->add_right_button("Undo", "Icons/Undo/undo-24.png", []() {});
	header_view->add_right_button("Redo", "Icons/Redo/redo-24.png", []() {});
	
	header_view->add_right_button("Change Model", "Icons/Model/model-24.png", bind_member(this, &ModelEditorWindow::on_change_model));

	auto options = header_view->add_right_menu("Options", "Icons/Menu/menu-24.png", true);
	options->add_item("Attachments", bind_member(this, &ModelEditorWindow::on_show_attachments));
	options->add_item("Animations", bind_member(this, &ModelEditorWindow::on_show_animations));
	options->add_item("Materials", bind_member(this, &ModelEditorWindow::on_show_materials));
	options->add_item("Lights", bind_member(this, &ModelEditorWindow::on_show_lights));
	options->add_item("Bones", bind_member(this, &ModelEditorWindow::on_show_bones));
	options->add_item("Cameras", bind_member(this, &ModelEditorWindow::on_show_cameras));
	options->add_item("Environment", bind_member(this, &ModelEditorWindow::on_show_environment));

	scene_controller = std::make_shared<SceneController>();
	animations_controller = std::make_shared<AnimationsController>();
	attachments_controller = std::make_shared<AttachmentsController>();
	materials_controller = std::make_shared<MaterialsController>();
	lights_controller = std::make_shared<LightsController>();
	bones_controller = std::make_shared<BonesController>();
	cameras_controller = std::make_shared<CamerasController>();
	environment_controller = std::make_shared<EnvironmentController>();

	workspace_controller->set_center(scene_controller);
	workspace_controller->set_docked(environment_controller);
}

void ModelEditorWindow::create_layout()
{
	DisplayWindowDescription desc;
	desc.set_size(Size(1280, 768), false);
	desc.set_title("Scene3D Model Editor");
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

void ModelEditorWindow::update_window_title()
{
	if (!ModelAppModel::instance()->open_filename.empty())
		window_view()->get_display_window().set_title(PathHelp::get_basename(ModelAppModel::instance()->open_filename) + " - Scene3D Model Editor");
	else
		window_view()->get_display_window().set_title("Model Editor");
}

void ModelEditorWindow::on_open()
{
	OpenFileDialog dialog(view.get());
	dialog.set_title("Select Model");
	dialog.add_filter("Model description files (*.modeldesc)", "*.modeldesc", true);
	dialog.add_filter("All files (*.*)", "*.*", false);
	dialog.set_filename(ModelAppModel::instance()->open_filename);
	if (dialog.show())
	{
		ModelAppModel::instance()->open(dialog.get_filename());
		update_window_title();
	}
}

void ModelEditorWindow::on_save()
{
	if (ModelAppModel::instance()->open_filename.empty())
	{
		on_save_as();
	}
	else
	{
		ModelAppModel::instance()->save(ModelAppModel::instance()->open_filename);
	}
}

void ModelEditorWindow::on_save_as()
{
	SaveFileDialog dialog(view.get());
	dialog.set_title("Save Model Description");
	dialog.add_filter("Model description files (*.modeldesc)", "*.modeldesc", true);
	dialog.add_filter("All files (*.*)", "*.*", false);
	dialog.set_filename(ModelAppModel::instance()->open_filename);
	if (dialog.show())
	{
		std::string filename = dialog.get_filename();
		if (PathHelp::get_extension(filename).empty())
			filename += ".modeldesc";
		ModelAppModel::instance()->save(filename);
		update_window_title();
	}
}

void ModelEditorWindow::on_compile()
{
	if (ModelAppModel::instance()->open_filename.empty())
		return;

	on_save();

	AssetCompiler compiler;
	compiler.compile(ModelAppModel::instance()->open_filename, [](const CompilerMessage &msg) { });
}

void ModelEditorWindow::on_change_model()
{
	OpenFileDialog dialog(view.get());
	dialog.set_title("Select Model");
	dialog.add_filter("Autodesk FBX files (*.fbx)", "*.fbx", true);
	dialog.add_filter("All files (*.*)", "*.*", false);
	dialog.set_filename(ModelAppModel::instance()->desc.fbx_filename);
	if (dialog.show())
	{
		ModelAppModel::instance()->set_fbx_model(dialog.get_filename());
	}
}

void ModelEditorWindow::on_show_attachments()
{
	workspace_controller->set_docked(attachments_controller);
}

void ModelEditorWindow::on_show_animations()
{
	workspace_controller->set_docked(animations_controller);
}

void ModelEditorWindow::on_show_materials()
{
	workspace_controller->set_docked(materials_controller);
}

void ModelEditorWindow::on_show_lights()
{
	workspace_controller->set_docked(lights_controller);
}

void ModelEditorWindow::on_show_bones()
{
	workspace_controller->set_docked(bones_controller);
}

void ModelEditorWindow::on_show_cameras()
{
	workspace_controller->set_docked(cameras_controller);
}

void ModelEditorWindow::on_show_environment()
{
	workspace_controller->set_docked(environment_controller);
}
