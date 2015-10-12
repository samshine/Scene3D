
#pragma once

class ScreenViewController
{
public:
	ScreenViewController(const uicore::CanvasPtr &canvas);

	std::shared_ptr<uicore::TextureWindow> texture_view;

	virtual bool cursor_hidden() { return false; }
	virtual void update_desktop(const uicore::CanvasPtr &canvas, const uicore::DisplayWindowPtr &ic, const uicore::Vec2i &mouse_delta) { }

	void render_scene(const uicore::CanvasPtr &canvas, Scene &scene);
};

class Screen
{
public:
	static std::shared_ptr<ScreenViewController> &controller();

	static SceneCachePtr &scene_cache();
	static std::shared_ptr<SoundCache> &sound_cache();
};
