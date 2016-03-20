
#pragma once

class FrameStatistics
{
public:
	int frame_ms = 0;
	int models_drawn = 0;
	int instances_drawn = 0;
	int draw_calls = 0;
	int triangles_drawn = 0;
	int scene_visits = 0;
	std::vector<GPUTimer::Result> gpu_results;
	std::vector<std::string> cpu_results;
};

class ScreenViewController
{
public:
	virtual void update() { }

	const uicore::DisplayWindowPtr &window() const;
	uicore::Vec2i mouse_delta() const;
	const uicore::GameTime game_time() const;

	const uicore::GraphicContextPtr &gc() const;
	const uicore::CanvasPtr &canvas() const;

	const SceneEnginePtr &scene_engine() const;
	const SceneViewportPtr &scene_viewport() const;

	const std::shared_ptr<SoundCache> &sound_cache() const;

	void set_cursor_hidden(bool value = true) { _cursor_hidden = value; }
	bool cursor_hidden() const { return _cursor_hidden; }

	void present_controller(std::shared_ptr<ScreenViewController> controller);
	void exit_game();

protected:
	uicore::SlotContainer slots;

private:
	bool _cursor_hidden = false;
};

class MouseMovement;

class Screen
{
public:
	static Screen *instance();

	void run();

	const FrameStatistics &last_frame_stats() { return instance()->frame_stats; }

private:
	uicore::DisplayWindowPtr window;
	std::shared_ptr<MouseMovement> mouse_movement;

	uicore::GraphicContextPtr gc;
	uicore::CanvasPtr canvas;

	SoundOutput sound_output;
	std::shared_ptr<SoundCache> sound_cache;

	SceneEnginePtr scene_engine;
	SceneViewportPtr scene_viewport;

	std::shared_ptr<ScreenViewController> screen_controller;

	uicore::GameTime game_time;
	uicore::Vec2i delta_mouse_move;

	int64_t last_fps_timestamp = uicore::System::microseconds();
	FrameStatistics frame_stats;

	friend class ScreenViewController;
	friend class ScreenView;
};
