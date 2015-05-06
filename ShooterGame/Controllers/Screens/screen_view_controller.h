
#pragma once

class ScreenViewController : public clan::ViewController
{
public:
	ScreenViewController(clan::Canvas &canvas);

	std::shared_ptr<clan::TextureView> texture_view();
	virtual bool cursor_hidden() { return false; }
	virtual void update_desktop(clan::Canvas &canvas, clan::InputContext &ic, const clan::Vec2i &mouse_delta) { }
	clan::Scene create_scene(clan::Canvas &canvas) { return clan::Scene(canvas.get_gc(), clan::UIThread::get_resources(), "Resources/Scene3D"); }

	void render_scene(clan::Canvas &canvas, clan::Scene &scene)
	{
		using namespace clan;

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
