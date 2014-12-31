
#include "precomp.h"
#include "scene_view.h"
#include "viewer_scene_cache.h"

using namespace clan;

SceneView::SceneView()
{
	box_style.set_layout_hbox();
	box_style.set_flex(1.0f, 1.0f);

	set_focus_policy(FocusPolicy::accept);

	slots.connect(sig_pointer_press(), [this](PointerEvent &e)
	{
		set_focus();
	});

	timer = Timer();
	timer.func_expired() = [this]()
	{
		set_needs_render();
	};
}

void SceneView::render_content(Canvas &canvas)
{
	Pointf viewport_pos = Vec2f(canvas.get_transform() * Vec4f(0.0f, 0.0f, 0.0f, 1.0f));
	Sizef viewport_size = geometry().content.get_size();
	//Size viewport_size_i = Size(2400*2 + rand() % 160 * 2, 1300*2 + rand() % 160 * 2);
	Size viewport_size_i = Size(viewport_size) * 2;

	canvas.flush();
	GraphicContext gc = canvas.get_gc();
	InputContext ic;
	if (dynamic_cast<WindowView*>(root_view())) ic = static_cast<WindowView*>(root_view())->get_display_window().get_ic();

	setup_scene(gc);
	sig_update_scene(scene, gc, ic);

	if (scene_frame_buffer.is_null() || scene_texture.is_null() || scene_texture.get_size() != viewport_size_i)
	{
		scene_frame_buffer = FrameBuffer();
		scene_texture = Texture2D();
		gc.flush();

		scene_texture = Texture2D(gc, viewport_size_i);
		scene_frame_buffer = FrameBuffer(gc);
		scene_frame_buffer.attach_color(0, scene_texture);
	}

	//scene.set_viewport(RectfPS(viewport_pos.x, viewport_pos.y, viewport_size.width, viewport_size.height));
	scene.set_viewport(viewport_size_i, scene_frame_buffer);
	scene.render(gc);

	gc.set_viewport(gc.get_size());

	canvas.fill_triangles(
		{
			Vec2f(0.0f, 0.0f), Vec2f(viewport_size_i.width / 2.0f, 0.0f), Vec2f(0.0f, viewport_size_i.height / 2.0f),
			Vec2f(viewport_size_i.width / 2.0f, 0.0f), Vec2f(0.0f, viewport_size_i.height / 2.0f), Vec2f(viewport_size_i.width / 2.0f, viewport_size_i.height / 2.0f)
		}, scene_texture);

	timer.start(10, false);
}

void SceneView::setup_scene(GraphicContext &gc)
{
	if (!scene.is_null()) return;

	resources = ResourceManager();
	SceneCache::set(resources, std::shared_ptr<SceneCache>(new ViewerSceneCache()));

	scene = Scene(gc, resources, "Resources/Scene3D");

	scene.show_skybox_stars(false);
	std::vector<Colorf> gradient;
	gradient.push_back(Colorf(236 * 5 / 10, 240 * 5 / 10, 243 * 5 / 10));
	gradient.push_back(Colorf(236 * 5 / 10, 240 * 5 / 10, 243 * 5 / 10));
	gradient.push_back(Colorf(236 * 5 / 10, 240 * 5 / 10, 243 * 5 / 10));
	gradient.push_back(Colorf(236 * 5 / 10, 240 * 5 / 10, 243 * 5 / 10));
	gradient.push_back(Colorf(236 * 5 / 10, 240 * 5 / 10, 243 * 5 / 10));
	gradient.push_back(Colorf(236 * 5 / 10, 240 * 5 / 10, 243 * 5 / 10));
	gradient.push_back(Colorf(236 * 6 / 10, 240 * 6 / 10, 243 * 6 / 10));
	gradient.push_back(Colorf(236 * 7 / 10, 240 * 7 / 10, 243 * 7 / 10));
	gradient.push_back(Colorf(236 * 8 / 10, 240 * 8 / 10, 243 * 8 / 10));
	for (auto &g : gradient)
	{
		g.r = std::pow(g.r, 2.2f);
		g.g = std::pow(g.g, 2.2f);
		g.b = std::pow(g.b, 2.2f);
	}
	scene.set_skybox_gradient(gc, gradient);
}
