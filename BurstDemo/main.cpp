
#include "precomp.h"
#include "demo_screen_controller.h"
#include "raytracer_screen_controller.h"

using namespace uicore;

int CALLBACK WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	Screen::instance()->run([]() { return std::make_shared<DemoScreenController>(); }, "Scene3D Burst Demo");
	//Screen::instance()->run([]() { return std::make_shared<RaytracerScreenController>(); }, "Scene3D Burst Demo");
	return 0;
}
