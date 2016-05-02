
#include "precomp.h"
#include "Screen/screen.h"
#include "Screen/screen_controller.h"
#include "game_time.h"

const uicore::DisplayWindowPtr &ScreenController::window() const
{
	return Screen::instance()->window();
}

uicore::Vec2i ScreenController::mouse_delta() const
{
	return Screen::instance()->delta_mouse_move();
}

const GameTime ScreenController::game_time() const
{
	return Screen::instance()->game_time();
}

const uicore::GraphicContextPtr &ScreenController::gc() const
{
	return Screen::instance()->gc();
}

const uicore::CanvasPtr &ScreenController::canvas() const
{
	return Screen::instance()->canvas();
}

const SceneEnginePtr &ScreenController::scene_engine() const
{
	return Screen::instance()->scene_engine();
}

const SceneViewportPtr &ScreenController::scene_viewport() const
{
	return Screen::instance()->scene_viewport();
}

const std::shared_ptr<SoundCache> &ScreenController::sound_cache() const
{
	return Screen::instance()->sound_cache();
}

void ScreenController::present_controller(std::shared_ptr<ScreenController> controller)
{
	Screen::instance()->screen_controller() = controller;
}

void ScreenController::exit_game()
{
	Screen::instance()->screen_controller().reset();
}
