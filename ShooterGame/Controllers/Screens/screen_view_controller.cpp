
#include "precomp.h"
#include "screen_view_controller.h"

using namespace uicore;

ScreenViewController::ScreenViewController(Canvas &canvas)
{
	texture_view = std::make_shared<uicore::TextureWindow>(canvas);
	texture_view->set_viewport(canvas.get_size());
	texture_view->set_clear_background(false);
	texture_view->set_always_render(true);
}

void ScreenViewController::render_scene(uicore::Canvas &canvas, Scene &scene)
{
	using namespace uicore;

	GraphicContext gc = canvas.get_gc();

	Pointf viewport_pos = Vec2f(canvas.get_transform() * Vec4f(0.0f, 0.0f, 0.0f, 1.0f));
	Sizef viewport_size = gc.get_size();
	Size viewport_size_i = Size(viewport_size);

	scene.set_viewport(viewport_size_i);
	scene.render(gc);

	gc.set_viewport(gc.get_size());

	canvas.reset_blend_state();
}

/////////////////////////////////////////////////////////////////////////////

std::shared_ptr<ScreenViewController> &Screen::controller()
{
	static std::shared_ptr<ScreenViewController> screen_controller;
	return screen_controller;
}

SceneCache &Screen::scene_cache()
{
	static SceneCache cache;
	return cache;
}

std::shared_ptr<SoundCache> &Screen::sound_cache()
{
	static std::shared_ptr<SoundCache> cache;
	return cache;
}
