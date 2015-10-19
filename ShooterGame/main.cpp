
#include "precomp.h"
#include "Controllers/Screens/screen_view_controller.h"
#include "Controllers/Screens/menu_screen_controller.h"
#include "Controllers/Screens/game_screen_controller.h"
#include "mouse_movement.h"

using namespace uicore;

int CALLBACK WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	srand((unsigned int)System::get_time());

	D3DTarget::set_current();

	DisplayWindowDescription window_desc;
	window_desc.set_title("Scene3D Shooter Game");
	window_desc.set_size(Sizef(1920.0f, 1280.0f), true);
	window_desc.set_allow_resize();
	window_desc.set_visible(false);

	DisplayWindowPtr window = DisplayWindow::create(window_desc);
	GraphicContextPtr gc = window->gc();
	CanvasPtr canvas = Canvas::create(window);

	window->set_large_icon(PNGFormat::load("Resources/Icons/App/AppIcon-128.png"));
	window->set_small_icon(PNGFormat::load("Resources/Icons/App/AppIcon-128.png"));

	Slot close_slot = window->sig_window_close().connect([]() { RunLoop::exit(); });

	window->maximize();
	window->show();

	SoundOutput sound_output(44100);

	MouseMovement mouse_movement;

	Screen::scene_engine() = SceneEngine::create(gc, "Resources/Scene3D");
	Screen::sound_cache() = std::make_shared<SoundCache>();

	SlotContainer slots;
	slots.connect(window->keyboard()->sig_key_down(), [&](const InputEvent &e) {
		auto screen = Screen::controller();
		if (screen) screen->texture_view->on_key_down(e);
	});
	slots.connect(window->keyboard()->sig_key_up(), [&](const InputEvent &e) {
		auto screen = Screen::controller();
		if (screen) screen->texture_view->on_key_down(e);
	});
	slots.connect(window->mouse()->sig_key_down(), [&](const InputEvent &e) {
		auto screen = Screen::controller();
		if (screen) screen->texture_view->on_mouse_down(e);
	});
	slots.connect(window->mouse()->sig_key_up(), [&](const InputEvent &e) {
		auto screen = Screen::controller();
		if (screen) screen->texture_view->on_mouse_up(e);
	});
	slots.connect(window->mouse()->sig_pointer_move(), [&](const InputEvent &e) {
		auto screen = Screen::controller();
		if (screen) screen->texture_view->on_mouse_move(e);
	});

	//ConsoleWindow console("Debug");

	try
	{
		Screen::controller() = std::make_shared<MenuScreenController>(canvas);

		Vec2i last_mouse_movement = mouse_movement.pos();
		bool cursor_hidden = false;
		Pointf mouse_down_pos;

		while (RunLoop::process())
		{
			gc->clear();

			canvas->begin();

			auto screen = Screen::controller();
			if (screen)
			{
				bool hide_cursor = screen->cursor_hidden();
				if (cursor_hidden != hide_cursor)
				{
					if (hide_cursor)
					{
						window->hide_cursor();
						mouse_down_pos = window->mouse()->position();
					}
					else
					{
						window->mouse()->set_position(mouse_down_pos.x, mouse_down_pos.y);
						window->show_cursor();
					}
					cursor_hidden = hide_cursor;
				}

				if (cursor_hidden)
				{
					Sizef size = canvas->size();
					window->mouse()->set_position(size.width * 0.5f, size.height * 0.5f);
				}

				Point move = mouse_movement.pos();
				Vec2i delta_mouse_move = move - last_mouse_movement;
				last_mouse_movement = move;

				screen->update_desktop(canvas, window, delta_mouse_move);
				screen->texture_view->set_viewport(canvas->size());
				screen->texture_view->update();
			}

			canvas->end();
			window->flip(1);
		}

		Screen::controller().reset();
	}
	catch (...)
	{
		ExceptionDialog::show(std::current_exception());
	}

	return 0;
}
