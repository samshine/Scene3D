
#pragma once

class ScreenViewController
{
public:
	ScreenViewController(uicore::Canvas &canvas);

	std::shared_ptr<uicore::TextureWindow> texture_view;

	virtual bool cursor_hidden() { return false; }
	virtual void update_desktop(uicore::Canvas &canvas, const uicore::DisplayWindowPtr &ic, const uicore::Vec2i &mouse_delta) { }

	void render_scene(uicore::Canvas &canvas, Scene &scene);
};

class Screen
{
public:
	static std::shared_ptr<ScreenViewController> &controller();

	static SceneCache &scene_cache();
	static std::shared_ptr<SoundCache> &sound_cache();
};
