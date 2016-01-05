
#include "precomp.h"
#include "screen_view.h"
#include "Controllers/Screens/screen_view_controller.h"

const uicore::DisplayWindowPtr &ScreenView::window() const
{
	return Screen::instance()->window;
}

uicore::Vec2i ScreenView::mouse_delta() const
{
	return Screen::instance()->delta_mouse_move;
}

const uicore::GameTime ScreenView::game_time() const
{
	return Screen::instance()->game_time;
}

const uicore::GraphicContextPtr &ScreenView::gc() const
{
	return Screen::instance()->gc;
}

const uicore::CanvasPtr &ScreenView::canvas() const
{
	return Screen::instance()->canvas;
}

const SceneEnginePtr &ScreenView::scene_engine() const
{
	return Screen::instance()->scene_engine;
}

const SceneViewportPtr &ScreenView::scene_viewport() const
{
	return Screen::instance()->scene_viewport;
}

const std::shared_ptr<SoundCache> &ScreenView::sound_cache() const
{
	return Screen::instance()->sound_cache;
}
