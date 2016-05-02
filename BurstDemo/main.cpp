
#include "precomp.h"
#include "demo_screen_controller.h"

using namespace uicore;

int CALLBACK WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	Screen::instance()->run([]() { return std::make_shared<DemoScreenController>(); }, "Scene3D Burst Demo");
	return 0;
}
