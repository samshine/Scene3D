
#include "precomp.h"
#include "ResourceCaches/game_display_cache.h"
#include "ResourceCaches/game_scene_cache.h"
#include "Controllers/Screens/screen_view_controller.h"
#include "Controllers/Screens/menu_screen_controller.h"

using namespace clan;

int CALLBACK WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	srand((unsigned int)System::get_time());

	D3DTarget::set_current();

	DisplayWindowDescription window_desc;
	window_desc.set_title("Scene3D Shooter Game");
	window_desc.set_size(Sizef(1920.0f, 1280.0f), true);
	window_desc.set_allow_resize();
	window_desc.set_visible(false);

	DisplayWindow window(window_desc);
	GraphicContext gc = window.get_gc();
	Canvas canvas(window);

	window.set_large_icon(PNGProvider::load("Resources/Icons/App/AppIcon-128.png"));
	window.set_small_icon(PNGProvider::load("Resources/Icons/App/AppIcon-128.png"));

	Slot close_slot = window.sig_window_close().connect([]() { RunLoop::exit(); });

	window.maximize();
	window.show();

	ResourceManager resources;
	DisplayCache::set(resources, std::make_shared<GameDisplayCache>());
	SceneCache::set(resources, std::make_shared<GameSceneCache>(gc));

	UIThread ui_thread(resources);

	try
	{
		Screen::set(std::make_shared<MenuScreenController>(canvas));

		while (RunLoop::process())
		{
			static_cast<GameSceneCache*>(&SceneCache::get(resources))->process_work_completed();

			gc.clear();

			Screen::get()->update_desktop(canvas);

			Screen::get()->texture_view()->set_viewport(canvas.get_size());
			Screen::get()->texture_view()->set_needs_render();
			Screen::get()->texture_view()->update();

			canvas.flush();
			window.flip(1);
		}

		Screen::set(std::shared_ptr<ScreenViewController>());
	}
	catch (...)
	{
		ExceptionDialog::show(std::current_exception());
	}

	return 0;
}