
#include "precomp.h"
#include "Screen/screen.h"
#include "Screen/screen_controller.h"
#include "scene3d.h"
#include "sound.h"
#include "mouse_movement.h"
#include "game_time.h"

using namespace uicore;

class ScreenImpl : public Screen
{
public:
	void run(std::function<std::shared_ptr<ScreenController>()> create_screen_controller, const std::string &title, const std::string &icon, const std::string &cursor) override;

	const uicore::DisplayWindowPtr &window() override { return _window; }
	const FrameStatistics &last_frame_stats() override { return _frame_stats; }

	GraphicContextPtr &gc() override { return _gc; }
	CanvasPtr &canvas() override { return _canvas; }

	SoundOutput &sound_output() override { return _sound_output; }
	std::shared_ptr<SoundCache> &sound_cache() override { return _sound_cache; }

	SceneEnginePtr &scene_engine() override { return _scene_engine; }
	SceneViewportPtr &scene_viewport() override { return _scene_viewport; }

	std::shared_ptr<ScreenController> &screen_controller() override { return _screen_controller; }

	GameTime &game_time() override { return _game_time; }
	Vec2i &delta_mouse_move() override { return _delta_mouse_move; }

private:
	DisplayWindowPtr _window;
	std::shared_ptr<MouseMovement> _mouse_movement;

	GraphicContextPtr _gc;
	CanvasPtr _canvas;

	SoundOutput _sound_output;
	std::shared_ptr<SoundCache> _sound_cache;

	SceneEnginePtr _scene_engine;
	SceneViewportPtr _scene_viewport;

	std::shared_ptr<ScreenController> _screen_controller;

	GameTime _game_time;
	Vec2i _delta_mouse_move;

	int64_t _last_fps_timestamp = System::microseconds();
	FrameStatistics _frame_stats;
};

Screen *Screen::instance()
{
	static Singleton<ScreenImpl> screen;
	return screen.get();
}

void ScreenImpl::run(std::function<std::shared_ptr<ScreenController>()> create_screen_controller, const std::string &title, const std::string &icon, const std::string &cursor_filename)
{
	try
	{
		srand((unsigned int)System::time());

		ScopeTimerResults scope_timer_results;

		D3DTarget::set_current();
		//OpenGLTarget::set_current();

		DisplayWindowDescription window_desc;
		window_desc.set_title(title);
		window_desc.set_size(Sizef(1920.0f, 1280.0f), true);
		window_desc.set_allow_resize();
		window_desc.set_visible(false);
		window_desc.set_allow_alt_enter();

		_window = DisplayWindow::create(window_desc);
		_gc = _window->gc();
		_canvas = Canvas::create(_window);

		if (!icon.empty())
		{
			_window->set_large_icon(PNGFormat::load(icon));
			_window->set_small_icon(PNGFormat::load(icon));
		}

		if (!cursor_filename.empty())
		{
			auto cursor = LoadCursorFromFile(Text::to_utf16(cursor_filename).c_str());
			if (cursor != 0)
				_window->set_cursor_handle(cursor);
		}

		Slot close_slot = _window->sig_window_close().connect([]() { RunLoop::exit(); });

		_window->maximize();
		_window->show();

		_gc->clear();
		_window->flip();

		_mouse_movement = std::make_shared<MouseMovement>();

		_sound_output = SoundOutput(48000);

		_scene_engine = SceneEngine::create();
		_scene_viewport = SceneViewport::create(_scene_engine);

		_sound_cache = std::make_shared<SoundCache>();

		_screen_controller = create_screen_controller();// std::make_shared<MenuScreenController>();

		Vec2i last_mouse_movement = _mouse_movement->pos();
		bool cursor_hidden = false;
		Pointf mouse_down_pos;

		_last_fps_timestamp = System::microseconds();

		while (RunLoop::process())
		{
			if (!_screen_controller)
				break;

			ScopeTimerResults::start();

			{
				ScopeTimer scope_timer("SceneEngine::wait_next_frame_ready");
				_scene_engine->wait_next_frame_ready(_gc);
			}

			bool hide_cursor = _screen_controller->cursor_hidden() && _window->has_focus();
			if (cursor_hidden != hide_cursor)
			{
				if (hide_cursor)
				{
					_window->hide_cursor();
					mouse_down_pos = _window->mouse()->position();
				}
				else
				{
					_window->mouse()->set_position(mouse_down_pos.x, mouse_down_pos.y);
					_window->show_cursor();
				}
				cursor_hidden = hide_cursor;
			}

			if (cursor_hidden && _window->has_focus())
			{
				Sizef size = _canvas->size();
				_window->mouse()->set_position(size.width * 0.5f, size.height * 0.5f);
			}

			Point move = _mouse_movement->pos();
			_delta_mouse_move = move - last_mouse_movement;
			if (!_window->has_focus() || !_screen_controller->cursor_hidden())
				_delta_mouse_move = Vec2i();
			last_mouse_movement = move;

			_scene_viewport->set_viewport(_gc->size(), nullptr, _gc->pixel_ratio() >= 2.0f ? 2.0f : 1.0f);

			_game_time.update();

			{
				ScopeTimer scope_timer("ScreenController::update");
				_screen_controller->update();
			}

			{
				ScopeTimer scope_timer("DisplayWindow::flip");
				_window->flip(1);
			}

			ScopeTimerResults::end();

			int64_t timestamp = System::microseconds();
			int frame_ms = (int)((timestamp - _last_fps_timestamp) / 1000);
			_last_fps_timestamp = timestamp;

			FrameStatistics stats;
			stats.frame_ms = frame_ms;
			stats.cpu_results = ScopeTimerResults::timer_results();
			stats.gpu_results = _scene_engine->gpu_results();
			stats.models_drawn = _scene_engine->models_drawn();
			stats.instances_drawn = _scene_engine->instances_drawn();
			stats.draw_calls = _scene_engine->draw_calls();
			stats.triangles_drawn = _scene_engine->triangles_drawn();
			stats.scene_visits = _scene_engine->scene_visits();
			stats.model_instance_maps = _scene_engine->model_instance_maps();
			_frame_stats = stats;
		}
	}
	catch (...)
	{
		ExceptionDialog::show(std::current_exception());
	}

	SingletonBugfix::deinitialize();
}
