
#pragma once

class ScreenViewController
{
public:
	ScreenViewController(uicore::Canvas &canvas);

	std::shared_ptr<uicore::TextureWindow> texture_view;

	virtual bool cursor_hidden() { return false; }
	virtual void update_desktop(uicore::Canvas &canvas, uicore::DisplayWindow &ic, const uicore::Vec2i &mouse_delta) { }

	uicore::Scene create_scene(uicore::Canvas &canvas);
	void render_scene(uicore::Canvas &canvas, uicore::Scene &scene);
};

class Screen
{
public:
	static std::shared_ptr<ScreenViewController> &controller();

	static const uicore::ResourceManager &resources();
	static void set_resources(const uicore::ResourceManager &resources);

	static std::shared_ptr<uicore::SceneCache> &scene_cache();
	static std::shared_ptr<uicore::SoundCache> &sound_cache();

private:
	static uicore::ResourceManager resource_manager;
};
