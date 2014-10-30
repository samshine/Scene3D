
#include "precomp.h"
#include "main_window.h"
#include "Header/header_view.h"
#include "Scene/scene_view.h"
#include "Dock/dock_view.h"

using namespace clan;

MainWindow::MainWindow() : WindowView(create_desc())
{
	box_style.set_layout_vbox();
	box_style.set_background(Colorf(240, 240, 240));
	slots.connect(sig_close(), [this](CloseEvent &e)
	{
		exit = true;
	});

	header_view = std::make_shared<HeaderView>();
	scene_view = std::make_shared<SceneView>();
	dock_view = std::make_shared<DockView>();

	auto workspace = std::make_shared<View>();
	workspace->box_style.set_layout_hbox();
	workspace->box_style.set_flex(1.0f, 0.0f);
	workspace->add_subview(scene_view);
	workspace->add_subview(dock_view);

	add_subview(header_view);
	add_subview(workspace);
}

DisplayWindowDescription MainWindow::create_desc()
{
	DisplayWindowDescription desc;
	desc.set_size(Size(1280, 768), false);
	desc.set_title("FBX Viewer");
	desc.set_allow_resize(true);
	return desc;
}
