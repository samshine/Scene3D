
#include "precomp.h"
#include "Views/MainWindow/main_window.h"
#include "Model/app_model.h"

int CALLBACK WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	clan::SetupCore core;
	clan::SetupDisplay display;
	clan::SetupD3D d3d;

	AppModel model;

	model.open("Resources/Thalania/Thalania.fbx");

	auto window = std::make_shared<MainWindow>();
	window->show();

	RunLoop::run();
	return 0;
}

