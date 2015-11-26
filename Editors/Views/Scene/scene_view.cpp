
#include "precomp.h"
#include "scene_view.h"

using namespace uicore;

SceneView::SceneView()
{
	style()->set("flex: auto");

	set_focus_policy(FocusPolicy::accept);

	slots.connect(sig_pointer_press(), this, &SceneView::pointer_press);
	slots.connect(sig_pointer_release(), this, &SceneView::pointer_release);
	slots.connect(sig_pointer_move(), this, &SceneView::pointer_move);

	timer = Timer::create();
	timer->func_expired() = [this]()
	{
		set_needs_render();
	};
}

void SceneView::pointer_press(PointerEvent &e)
{
	set_focus();

	if (!mouse_down)
	{
		view_tree()->get_display_window()->hide_cursor();
		mouse_down_pos = view_tree()->get_display_window()->mouse()->position();
		mouse_down = true;
	}
}

void SceneView::pointer_release(PointerEvent &e)
{
	if (mouse_down)
	{
		view_tree()->get_display_window()->mouse()->set_position(mouse_down_pos.x, mouse_down_pos.y);
		view_tree()->get_display_window()->show_cursor();
		mouse_down = false;
	}
}

void SceneView::pointer_move(PointerEvent &e)
{
	if (mouse_down)
	{
		Sizef size = view_tree()->get_display_window()->geometry().size();
		view_tree()->get_display_window()->mouse()->set_position(size.width * 0.5f, size.height * 0.5f);
	}
}

void SceneView::render_content(const CanvasPtr &canvas)
{
	Pointf viewport_pos = Vec2f(canvas->transform() * Vec4f(0.0f, 0.0f, 0.0f, 1.0f));
	Sizef viewport_size = geometry().content_size();
	//Size viewport_size_i = Size(2400*2 + rand() % 160 * 2, 1300*2 + rand() % 160 * 2);
	Size viewport_size_i = Size(viewport_size) * 2;
	bool supersampling = true;
	/*if (viewport_size_i.width > 3000) // Disable multisample for high resolutions
	{
		viewport_size_i = Size(viewport_size);
		supersampling = false;
	}*/

	viewport_size_i.width = std::max(viewport_size_i.width, 16);
	viewport_size_i.height = std::max(viewport_size_i.height, 16);

	canvas->end();

	auto gc = canvas->gc();
	auto window = view_tree()->get_display_window();

	Point move = mouse_movement.pos();
	Vec2i delta_mouse_move;
	if (mouse_down)
	{
		delta_mouse_move = move - last_mouse_movement;
	}
	last_mouse_movement = move;


	setup_scene(gc);
	sig_update_scene(scene, scene_viewport, gc, window, delta_mouse_move);

	if (!scene_frame_buffer || !scene_texture || scene_texture->size() != viewport_size_i)
	{
		scene_frame_buffer.reset();
		scene_texture.reset();
		gc->flush();

		scene_texture = Texture2D::create(gc, viewport_size_i);
		scene_frame_buffer = FrameBuffer::create(gc);
		scene_frame_buffer->attach_color(0, scene_texture);
	}

	//scene_viewport->set_viewport(RectfPS(viewport_pos.x, viewport_pos.y, viewport_size.width, viewport_size.height));
	scene_viewport->set_viewport(viewport_size_i, scene_frame_buffer);
	scene_viewport->render(gc);

	gc->set_viewport(gc->size(), y_axis_top_down);

	canvas->begin();

	auto image = Image::create(scene_texture, scene_texture->size(), gc->pixel_ratio());
	image->draw(canvas, geometry().content_size());

	timer->start(10, true);
}

void SceneView::setup_scene(const GraphicContextPtr &gc)
{
	if (scene) return;

	cache = SceneEngine::create(gc);
	scene = Scene::create(cache);
	scene_viewport = SceneViewport::create(cache);

	scene_viewport->set_camera(SceneCamera::create(scene));

	scene->show_skybox_stars(false);
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
	scene->set_skybox_gradient(gradient);
}
