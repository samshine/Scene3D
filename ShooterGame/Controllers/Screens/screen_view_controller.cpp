
#include "precomp.h"
#include "screen_view_controller.h"
#include "../../mouse_movement.h"
#include "menu_screen_controller.h"
#include "game_screen_controller.h"

using namespace uicore;

const uicore::DisplayWindowPtr &ScreenViewController::window() const
{
	return Screen::instance()->window;
}

uicore::Vec2i ScreenViewController::mouse_delta() const
{
	return Screen::instance()->delta_mouse_move;
}

const uicore::GameTime ScreenViewController::game_time() const
{
	return Screen::instance()->game_time;
}

const uicore::GraphicContextPtr &ScreenViewController::gc() const
{
	return Screen::instance()->gc;
}

const uicore::CanvasPtr &ScreenViewController::canvas() const
{
	return Screen::instance()->canvas;
}

const SceneEnginePtr &ScreenViewController::scene_engine() const
{
	return Screen::instance()->scene_engine;
}

const SceneViewportPtr &ScreenViewController::scene_viewport() const
{
	return Screen::instance()->scene_viewport;
}

const std::shared_ptr<SoundCache> &ScreenViewController::sound_cache() const
{
	return Screen::instance()->sound_cache;
}

void ScreenViewController::present_controller(std::shared_ptr<ScreenViewController> controller)
{
	Screen::instance()->screen_controller = controller;
}

void ScreenViewController::exit_game()
{
	Screen::instance()->screen_controller.reset();
}

void Screen::run()
{
	try
	{
		srand((unsigned int)System::time());

		ScopeTimerResults scope_timer_results;

		D3DTarget::set_current();
		//OpenGLTarget::set_current();

		DisplayWindowDescription window_desc;
		window_desc.set_title("Scene3D Shooter Game");
		window_desc.set_size(Sizef(1920.0f, 1280.0f), true);
		window_desc.set_allow_resize();
		window_desc.set_visible(false);
		window_desc.set_allow_alt_enter();

		window = DisplayWindow::create(window_desc);
		gc = window->gc();
		canvas = Canvas::create(window);

		window->set_large_icon(PNGFormat::load("Resources/Icons/App/AppIcon-128.png"));
		window->set_small_icon(PNGFormat::load("Resources/Icons/App/AppIcon-128.png"));

		auto cursor = LoadCursorFromFile(Text::to_utf16("Resources/Cursors/Blacknglow/normal select.cur").c_str());
		if (cursor != 0)
			window->set_cursor_handle(cursor);

		Slot close_slot = window->sig_window_close().connect([]() { RunLoop::exit(); });

		window->maximize();
		window->show();

		gc->clear();
		window->flip();

		mouse_movement = std::make_shared<MouseMovement>();

		sound_output = SoundOutput(48000);

		scene_engine = SceneEngine::create();
		scene_viewport = SceneViewport::create(scene_engine);

		sound_cache = std::make_shared<SoundCache>();

		screen_controller = std::make_shared<MenuScreenController>();

		Vec2i last_mouse_movement = mouse_movement->pos();
		bool cursor_hidden = false;
		Pointf mouse_down_pos;

		while (RunLoop::process())
		{
			if (!screen_controller)
				break;

			ScopeTimerResults::start();

			bool hide_cursor = screen_controller->cursor_hidden() && window->has_focus();
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

			if (cursor_hidden && window->has_focus())
			{
				Sizef size = canvas->size();
				window->mouse()->set_position(size.width * 0.5f, size.height * 0.5f);
			}

			Point move = mouse_movement->pos();
			delta_mouse_move = move - last_mouse_movement;
			if (!window->has_focus() || !screen_controller->cursor_hidden())
				delta_mouse_move = Vec2i();
			last_mouse_movement = move;

			scene_viewport->set_viewport(gc->size());

			game_time.update();

			{
				ScopeTimer scope_timer("ScreenController::update");
				screen_controller->update();
			}

			{
				ScopeTimer scope_timer("DisplayWindow::flip");
				window->flip(1);
			}

			ScopeTimerResults::end();
		}
	}
	catch (...)
	{
		ExceptionDialog::show(std::current_exception());
	}

	SingletonBugfix::deinitialize();
}

Screen *Screen::instance()
{
	static Singleton<Screen> screen;
	return screen.get();
}
