
#include "precomp.h"
#include "screen_view_controller.h"

using namespace uicore;

ScreenViewController::ScreenViewController(const CanvasPtr &canvas)
{
	texture_view = std::make_shared<uicore::TextureWindow>(canvas);
	texture_view->set_viewport(canvas->size());
	texture_view->set_clear_background(false);
	texture_view->set_always_render(true);
}

void ScreenViewController::render_scene(const CanvasPtr &canvas, const ScenePtr &scene)
{
	using namespace uicore;

	canvas->end();

	GraphicContextPtr gc = canvas->gc();

	Pointf viewport_pos = Vec2f(canvas->transform() * Vec4f(0.0f, 0.0f, 0.0f, 1.0f));
	Sizef viewport_size = gc->size();
	Size viewport_size_i = Size(viewport_size);

	scene->set_viewport(viewport_size_i);
	scene->render(gc);

	canvas->begin();
}

/////////////////////////////////////////////////////////////////////////////

std::shared_ptr<ScreenViewController> &Screen::controller()
{
	static std::shared_ptr<ScreenViewController> screen_controller;
	return screen_controller;
}

SceneCachePtr &Screen::scene_cache()
{
	static SceneCachePtr cache;
	return cache;
}

std::shared_ptr<SoundCache> &Screen::sound_cache()
{
	static std::shared_ptr<SoundCache> cache;
	return cache;
}
