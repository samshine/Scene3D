
#pragma once

class ScreenViewController : public uicore::ViewController
{
public:
	ScreenViewController(uicore::Canvas &canvas);

	std::shared_ptr<uicore::TextureView> texture_view();
	virtual bool cursor_hidden() { return false; }
	virtual void update_desktop(uicore::Canvas &canvas, uicore::DisplayWindow &ic, const uicore::Vec2i &mouse_delta) { }
	uicore::Scene create_scene(uicore::Canvas &canvas) { return uicore::Scene(canvas.get_gc(), uicore::UIThread::get_resources(), "Resources/Scene3D"); }

	void render_scene(uicore::Canvas &canvas, uicore::Scene &scene)
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
};

class Screen
{
public:
	static void set(const std::shared_ptr<ScreenViewController> &active);
	static std::shared_ptr<ScreenViewController> get();

private:
	static std::shared_ptr<ScreenViewController> active;
};
