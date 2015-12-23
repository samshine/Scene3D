
#pragma once

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

	friend class ScreenViewController;
};
