
#include "main_window.h"
#include "Header/header_view.h"
#include "Scene/scene_view.h"
#include "Dock/dock_view.h"
#include <UICore/Window/run_loop.h>

using namespace uicore;

MainWindow::MainWindow() : WindowView(create_desc())
{
	style.set_layout_vbox();
	style.set_background(Colorf::rgb8(240, 240, 240));
	slots.connect(sig_close(), [](CloseEvent &e) { RunLoop::exit(); });

	header_view = std::make_shared<HeaderView>();
	scene_view = std::make_shared<SceneView>();
	dock_view = std::make_shared<DockView>();

	auto workspace = std::make_shared<View>();
	workspace->style.set_layout_hbox();
	workspace->style.set_flex(1.0f, 0.0f);
	workspace->add_subview(scene_view);
	workspace->add_subview(dock_view);

	add_subview(header_view);
	add_subview(workspace);
}

WindowDescription MainWindow::create_desc()
{
	WindowDescription desc;
	desc.set_size(1280.0f, 768.0f);
	desc.set_title("FBX Viewer");
	return desc;
}
