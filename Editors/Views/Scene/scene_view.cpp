
#include "precomp.h"
#include "scene_view.h"

using namespace uicore;

SceneView::SceneView()
{
	style()->set("flex: auto");

	set_focus_policy(FocusPolicy::accept);

	slots.connect(sig_pointer_press(), [this](PointerEvent *e) { pointer_press(e); });

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

void SceneView::pointer_press(PointerEvent *e)
{
	set_focus();
}

void SceneView::draw_bbox(uicore::Vec3f bbox_min, uicore::Vec3f bbox_max)
{
	Vec3f corners[] =
	{
		Vec3f(bbox_min.x, bbox_min.y, bbox_min.z),
		Vec3f(bbox_max.x, bbox_min.y, bbox_min.z),
		Vec3f(bbox_max.x, bbox_min.y, bbox_max.z),
		Vec3f(bbox_min.x, bbox_min.y, bbox_max.z),
		Vec3f(bbox_min.x, bbox_max.y, bbox_min.z),
		Vec3f(bbox_max.x, bbox_max.y, bbox_min.z),
		Vec3f(bbox_max.x, bbox_max.y, bbox_max.z),
		Vec3f(bbox_min.x, bbox_max.y, bbox_max.z),
	};
	Vec4i pairs[] =
	{
		Vec4i(0, 3, 1, 4),
		Vec4i(1, 0, 2, 5),
		Vec4i(2, 1, 3, 6),
		Vec4i(3, 2, 0, 7),
		Vec4i(4, 5, 7, 0),
		Vec4i(5, 6, 4, 1),
		Vec4i(6, 7, 5, 2),
		Vec4i(7, 4, 6, 3)
	};
	for (const auto &pair : pairs)
	{
		scene_viewport->draw_line(corners[pair.x], mix(corners[pair.x], corners[pair.y], 0.20f), Vec3f(1.0f, 1.0f, 1.0f));
		scene_viewport->draw_line(corners[pair.x], mix(corners[pair.x], corners[pair.z], 0.20f), Vec3f(1.0f, 1.0f, 1.0f));
		scene_viewport->draw_line(corners[pair.x], mix(corners[pair.x], corners[pair.w], 0.20f), Vec3f(1.0f, 1.0f, 1.0f));
	}
}

void SceneView::draw_grid()
{
	for (int i = -10; i <= 10; i++)
	{
		auto color = (i % 10) == 0 ? Vec3f(0.2f, 0.2f, 0.2f) : Vec3f(0.05f, 0.05f, 0.05f);
		scene_viewport->draw_line(Vec3f(i * 0.1f, 0.0f, -1.0f), Vec3f(i * 0.1f, 0.0f, 1.0f), color);
		scene_viewport->draw_line(Vec3f(-1.0f, 0.0f, i * 0.1f), Vec3f(1.0f, 0.0f, i * 0.1f), color);
	}
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
	auto window = view_tree()->display_window();

	if (!scene_frame_buffer || !scene_texture || scene_texture->size() != viewport_size_i)
	{
		scene_frame_buffer.reset();
		scene_texture.reset();
		gc->flush();

		scene_texture = Texture2D::create(gc, viewport_size_i);
		scene_frame_buffer = FrameBuffer::create(gc);
		scene_frame_buffer->attach_color(0, scene_texture);
	}

	scene_viewport->set_viewport(viewport_size_i, scene_frame_buffer);

	scene_viewport->clear_lines();
	_sig_update_scene(scene_viewport, gc, window);

	for (auto &action : actions())
	{
		auto scene_action = std::dynamic_pointer_cast<SceneViewAction>(action);
		if (scene_action)
			scene_action->draw(scene_viewport);
	}

	scene_viewport->render(gc);

	gc->set_viewport(gc->size(), y_axis_top_down);

	canvas->begin();

	auto image = Image::create(scene_texture, scene_texture->size(), gc->pixel_ratio());
	if (gc->texture_image_y_axis() == y_axis_top_down)
		image->draw(canvas, geometry().content_size());
	else
		image->draw(canvas, Quadf(Vec2f(0.0f, geometry().content_height), Vec2f(geometry().content_width, geometry().content_height), Vec2f(geometry().content_width, 0.0f), Vec2f(0.0f, 0.0f)));
}

void SceneView::set_animate(bool value)
{
	if (value)
		timer->start(10, true);
	else
		timer->stop();
}
