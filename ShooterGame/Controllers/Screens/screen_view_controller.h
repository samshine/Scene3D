
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
	static void set(const std::shared_ptr<ScreenViewController> &active);
	static std::shared_ptr<ScreenViewController> get();

	static const uicore::ResourceManager &resources();
	static void set_resources(const uicore::ResourceManager &resources);

private:
	static std::shared_ptr<ScreenViewController> active;
	static uicore::ResourceManager resource_manager;
};
