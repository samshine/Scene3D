
#include "precomp.h"
#include "Controllers/Screens/screen_view_controller.h"

using namespace uicore;

int CALLBACK WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	Screen::instance()->run();
	return 0;
}
