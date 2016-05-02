
#pragma once

class ScreenController;
class SoundOutput;
class SoundCache;
class SceneEngine;
class SceneViewport;
typedef std::shared_ptr<SceneEngine> SceneEnginePtr;
typedef std::shared_ptr<SceneViewport> SceneViewportPtr;
class GameTime;

class ScreenController
{
public:
	virtual void update() { }

	const uicore::DisplayWindowPtr &window() const;
	uicore::Vec2i mouse_delta() const;
	const GameTime game_time() const;

	const uicore::GraphicContextPtr &gc() const;
	const uicore::CanvasPtr &canvas() const;

	const SceneEnginePtr &scene_engine() const;
	const SceneViewportPtr &scene_viewport() const;

	const std::shared_ptr<SoundCache> &sound_cache() const;

	void set_cursor_hidden(bool value = true) { _cursor_hidden = value; }
	bool cursor_hidden() const { return _cursor_hidden; }

	void present_controller(std::shared_ptr<ScreenController> controller);
	void exit_game();

protected:
	uicore::SlotContainer slots;

private:
	bool _cursor_hidden = false;
};
