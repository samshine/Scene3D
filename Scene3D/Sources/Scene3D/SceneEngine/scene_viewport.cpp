
#include "precomp.h"
#include "scene_viewport_impl.h"
#include "Scene3D/Scene/scene_camera_impl.h"

using namespace uicore;

std::shared_ptr<SceneViewport> SceneViewport::create(const SceneEnginePtr &engine)
{
	return std::make_shared<SceneViewportImpl>(engine);
}

SceneViewportImpl::SceneViewportImpl(const SceneEnginePtr &engine) : _engine(std::dynamic_pointer_cast<SceneEngineImpl>(engine))
{
}

Mat4f SceneViewportImpl::world_to_eye() const
{
	Quaternionf inv_orientation = Quaternionf::inverse(_camera->orientation());
	return inv_orientation.to_matrix() * Mat4f::translate(_camera->position());
}

Mat4f SceneViewportImpl::eye_to_projection() const
{
	Size viewport_size = _viewport.size();
	return Mat4f::perspective(_camera->field_of_view(), viewport_size.width / (float)viewport_size.height, 0.1f, 1.e10f, handed_left, clip_negative_positive_w);
}

Mat4f SceneViewportImpl::world_to_projection() const
{
	return eye_to_projection() * world_to_eye();
}

void SceneViewportImpl::unproject(const Vec2i &screen_pos, Vec3f &out_ray_start, Vec3f &out_ray_direction)
{
	Size viewport_size = _viewport.size();

	float aspect = _viewport.width() / (float)_viewport.height();
	float field_of_view_y_degrees = _camera->field_of_view();
	float field_of_view_y_rad = (float)(field_of_view_y_degrees * PI / 180.0);
	float f = 1.0f / tan(field_of_view_y_rad * 0.5f);
	float rcp_f = 1.0f / f;
	float rcp_f_div_aspect = 1.0f / (f / aspect);

	Vec2f pos((float)(screen_pos.x - _viewport.left), (float)(_viewport.bottom - screen_pos.y));

	Vec2f normalized(pos.x * 2.0f / _viewport.width(), pos.y * 2.0f / _viewport.height());
	normalized -= 1.0f;

	Vec3f ray_direction(normalized.x * rcp_f_div_aspect, normalized.y * rcp_f, 1.0f);

	out_ray_start = _camera->position();
	out_ray_direction = _camera->orientation().rotate_vector(ray_direction);
}

void SceneViewportImpl::clear_lines()
{
	_scene_lines.points.clear();
	_scene_lines.colors.clear();
	_scene_lines.text_locations.clear();
	_scene_lines.text_offsets.clear();
	_scene_lines.text_lengths.clear();
	_scene_lines.text.clear();
}

void SceneViewportImpl::draw_line(const uicore::Vec3f &from, const uicore::Vec3f &to, const uicore::Vec3f &color)
{
	_scene_lines.points.push_back(from);
	_scene_lines.points.push_back(to);
	_scene_lines.colors.push_back(color);
	_scene_lines.colors.push_back(color);
}

void SceneViewportImpl::draw_3d_text(const uicore::Vec3f &location, const char *text)
{
	_scene_lines.text_locations.push_back(location);
	_scene_lines.text_offsets.push_back(_scene_lines.text.length());
	_scene_lines.text += text;
	_scene_lines.text_lengths.push_back(_scene_lines.text.length() - _scene_lines.text_offsets.back());
}

void SceneViewportImpl::set_viewport(const Rect &box, const FrameBufferPtr &fb)
{
	_viewport = box;
	_fb_viewport = fb;
}

void SceneViewportImpl::render(const GraphicContextPtr &gc)
{
	engine()->render_viewport(gc, this);
}

void SceneViewportImpl::update(const GraphicContextPtr &gc, float time_elapsed)
{
	engine()->update_viewport(gc, this, time_elapsed);
}

SceneImpl *SceneViewportImpl::scene() const
{
	if (_camera)
		return static_cast<SceneCameraImpl*>(_camera.get())->scene;
	else
		return nullptr;
}
