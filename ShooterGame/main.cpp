
#include "precomp.h"
#include "Controllers/Screens/menu_screen_controller.h"

using namespace uicore;

int CALLBACK WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	Screen::instance()->run([]() { return std::make_shared<MenuScreenController>(); }, "Scene3D Shooter Game", "Resources/Icons/App/AppIcon-128.png", "Resources/Cursors/Blacknglow/normal select.cur");
	return 0;
}
