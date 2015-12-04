
#include "precomp.h"
#include "scene_view.h"

using namespace uicore;

SceneView::SceneView()
{
	style()->set("flex: auto");

	set_focus_policy(FocusPolicy::accept);

	slots.connect(sig_pointer_press(), this, &SceneView::pointer_press);

	timer = Timer::create();
	timer->func_expired() = [this]()
	{
		set_needs_render();
	};
}

SceneEnginePtr SceneView::engine()
{
	static SceneEnginePtr engine = SceneEngine::create();
	return engine;
}

void SceneView::pointer_press(PointerEvent &e)
{
	set_focus();
}

void SceneView::unproject(const Vec2i &pos, Vec3f &out_ray_start, Vec3f &out_ray_direction)
{
	if (scene_texture && geometry().content_width > 0.0f && geometry().content_height > 0.0f)
	{
		viewport()->unproject(
			Vec2i(
				(int)std::round(pos.x * scene_texture->width() / geometry().content_width),
				(int)std::round(pos.y * scene_texture->height() / geometry().content_height)),
			out_ray_start, out_ray_direction);
	}
	else
	{
		out_ray_start = Vec3f();
		out_ray_direction = Vec3f();
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

	sig_update_scene(scene_viewport, gc, window);

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
}

void SceneView::set_animate(bool value)
{
	if (value)
		timer->start(10, true);
	else
		timer->stop();
}
