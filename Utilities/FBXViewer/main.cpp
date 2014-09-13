
#include <UICore/Targets/target.h>
#include <UICore/Window/run_loop.h>
#include <UICore/Window/window.h>
#include <Windows.h>
#include <ClanLib/core.h>
#include <ClanLib/display.h>
#include <ClanLib/d3d.h>
#include <ClanLib/scene3d.h>
#include "Views/MainWindow/main_window.h"
#include "Model/app_model.h"

using namespace uicore;

int CALLBACK WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	clan::SetupCore core;
	clan::SetupDisplay display;
	clan::SetupD3D d3d;
	use_clanlib_target();

	AppModel model;

	model.open("Resources/Thalania/Thalania.fbx");

	auto window = std::make_shared<MainWindow>();
	window->show();

	RunLoop::run();
	return 0;
}

