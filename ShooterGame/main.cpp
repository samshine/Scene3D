
#include "precomp.h"
#include "ResourceCaches/game_display_cache.h"
#include "ResourceCaches/game_sound_cache.h"
#include "ResourceCaches/game_scene_cache.h"
#include "Controllers/Screens/screen_view_controller.h"
#include "Controllers/Screens/menu_screen_controller.h"
#include "Controllers/Screens/game_screen_controller.h"
#include "mouse_movement.h"

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
	InputContext ic = window.get_ic();
	Canvas canvas(window);

	window.set_large_icon(PNGProvider::load("Resources/Icons/App/AppIcon-128.png"));
	window.set_small_icon(PNGProvider::load("Resources/Icons/App/AppIcon-128.png"));

	Slot close_slot = window.sig_window_close().connect([]() { RunLoop::exit(); });

	window.maximize();
	window.show();

	SoundOutput sound_output(44100);

	MouseMovement mouse_movement;

	ResourceManager resources;
	DisplayCache::set(resources, std::make_shared<GameDisplayCache>());
	SceneCache::set(resources, std::make_shared<GameSceneCache>(gc));
	SoundCache::set(resources, std::make_shared<GameSoundCache>());

	UIThread ui_thread(resources);

	SlotContainer slots;
	slots.connect(window.get_ic().get_keyboard().sig_key_down(), [&](const InputEvent &e) {
		auto screen = Screen::get();
		if (screen) screen->texture_view()->on_key_down(e);
	});
	slots.connect(window.get_ic().get_keyboard().sig_key_up(), [&](const InputEvent &e) {
		auto screen = Screen::get();
		if (screen) screen->texture_view()->on_key_down(e);
	});
	slots.connect(window.get_ic().get_mouse().sig_key_down(), [&](const InputEvent &e) {
		auto screen = Screen::get();
		if (screen) screen->texture_view()->on_mouse_down(e);
	});
	slots.connect(window.get_ic().get_mouse().sig_key_up(), [&](const InputEvent &e) {
		auto screen = Screen::get();
		if (screen) screen->texture_view()->on_mouse_up(e);
	});
	slots.connect(window.get_ic().get_mouse().sig_pointer_move(), [&](const InputEvent &e) {
		auto screen = Screen::get();
		if (screen) screen->texture_view()->on_mouse_move(e);
	});

	//ConsoleWindow console("Debug");

	try
	{
		Screen::set(std::make_shared<MenuScreenController>(canvas));
		//Screen::set(std::make_shared<GameScreenController>(canvas));

		Vec2i last_mouse_movement = mouse_movement.pos();
		bool cursor_hidden = false;
		Pointf mouse_down_pos;

		while (RunLoop::process())
		{
			static_cast<GameSceneCache*>(&SceneCache::get(resources))->process_work_completed();

			gc.clear();

			auto screen = Screen::get();
			if (screen)
			{
				bool hide_cursor = screen->cursor_hidden();
				if (cursor_hidden != hide_cursor)
				{
					if (hide_cursor)
					{
						window.hide_cursor();
						mouse_down_pos = ic.get_mouse().get_position();
					}
					else
					{
						ic.get_mouse().set_position(mouse_down_pos.x, mouse_down_pos.y);
						window.show_cursor();
					}
					cursor_hidden = hide_cursor;
				}

				if (cursor_hidden)
				{
					Sizef size = canvas.get_size();
					ic.get_mouse().set_position(size.width * 0.5f, size.height * 0.5f);
				}

				Point move = mouse_movement.pos();
				Vec2i delta_mouse_move = move - last_mouse_movement;
				last_mouse_movement = move;

				screen->update_desktop(canvas, ic, delta_mouse_move);
				screen->texture_view()->set_viewport(canvas.get_size());
				screen->texture_view()->set_needs_render();
				screen->texture_view()->update();
			}

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
