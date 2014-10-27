
#include "precomp.h"
#include "Views/MainWindow/main_window.h"
#include "Model/app_model.h"

using namespace clan;

int CALLBACK WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	SetupCore core;
	SetupDisplay display;
	SetupD3D d3d;

	AppModel model;

	model.open("Resources/Thalania/Thalania.fbx");

	auto window = std::make_shared<MainWindow>();
	window->show();

	while (!window->exit)
	{
		KeepAlive::process(250);
	}
	return 0;
}

