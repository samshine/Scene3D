
#include "precomp.h"
#include "scene_pivot_action.h"

using namespace uicore;

void ScenePivotAction::pointer_press(PointerEvent *e)
{
	if (e->button() != PointerButton::left || action_active())
		return;

	hit_mode = hit_test(e->pos(view()));
	if (hit_mode != -1)
		begin_action();
}

void ScenePivotAction::pointer_double_click(PointerEvent *e)
{
}

void ScenePivotAction::pointer_release(PointerEvent *e)
{
	if (e->button() != PointerButton::left)
		return;

	if (action_active())
		end_action();
}

void ScenePivotAction::pointer_move(PointerEvent *e)
{
	if (action_active())
	{

	}
	else
	{
		hit_mode = hit_test(e->pos(view()));
	}
}

void ScenePivotAction::draw(const SceneViewportPtr &viewport)
{
	lines.clear();

	switch (_pivot_type)
	{
	default: break;
	case ScenePivotType::move: draw_move_pivot(viewport, _pivot_pos); break;
	case ScenePivotType::rotate: draw_rotate_pivot(viewport, _pivot_pos); break;
	case ScenePivotType::scale: draw_scale_pivot(viewport, _pivot_pos); break;
	}
}

int ScenePivotAction::hit_test(const uicore::Vec2f &pos)
{
	for (auto &line : lines)
	{
		if (line.first.point_distance(pos) <= 5.0f)
			return line.second;
	}

	return -1;
}

void ScenePivotAction::draw_line(const SceneViewportPtr &viewport, uicore::Vec3f from, uicore::Vec3f to, uicore::Vec3f color, int hit_type)
{
	if (hit_type == hit_mode)
		color = Vec3f(1.0f, 1.0f, 0.2f);

	auto from_2d = viewport->project(from) * 0.5f; // 0.5 because we supersample (viewport texture is twice the size as the view geometry) in SceneView
	auto to_2d = viewport->project(to) * 0.5f;
	lines.push_back({ LineSegment2f(from_2d, to_2d), hit_type });
	viewport->draw_line(from, to, color);
}

void ScenePivotAction::draw_move_pivot(const SceneViewportPtr &scene_viewport, uicore::Vec3f pos)
{
	float pivot_scale = 0.02f;
	Vec3f pivot_pos = scene_viewport->camera()->position() + Vec3f::normalize(pos - scene_viewport->camera()->position()) * 0.2f;

	draw_line(scene_viewport, pivot_pos + Vec3f(0.1f, 0.0f, 0.0f) * pivot_scale, pivot_pos + Vec3f(0.8f, 0.0f, 0.0f) * pivot_scale, Vec3f(1.0f, 0.2f, 0.2f), 0);
	draw_line(scene_viewport, pivot_pos + Vec3f(0.0f, 0.1f, 0.0f) * pivot_scale, pivot_pos + Vec3f(0.0f, 0.8f, 0.0f) * pivot_scale, Vec3f(0.2f, 1.0f, 0.2f), 1);
	draw_line(scene_viewport, pivot_pos + Vec3f(0.0f, 0.0f, 0.1f) * pivot_scale, pivot_pos + Vec3f(0.0f, 0.0f, 0.8f) * pivot_scale, Vec3f(0.2f, 0.2f, 1.0f), 2);
	draw_line(scene_viewport, pivot_pos + Vec3f(0.25f, 0.0f, 0.0f) * pivot_scale, pivot_pos + Vec3f(0.25f, 0.0f, 0.25f) * pivot_scale, Vec3f(1.0f, 0.2f, 0.2f), 3);
	draw_line(scene_viewport, pivot_pos + Vec3f(0.25f, 0.0f, 0.0f) * pivot_scale, pivot_pos + Vec3f(0.25f, 0.25f, 0.0f) * pivot_scale, Vec3f(1.0f, 0.2f, 0.2f), 3);
	draw_line(scene_viewport, pivot_pos + Vec3f(0.0f, 0.25f, 0.0f) * pivot_scale, pivot_pos + Vec3f(0.25f, 0.25f, 0.0f) * pivot_scale, Vec3f(0.2f, 1.0f, 0.2f), 4);
	draw_line(scene_viewport, pivot_pos + Vec3f(0.0f, 0.25f, 0.0f) * pivot_scale, pivot_pos + Vec3f(0.0f, 0.25f, 0.25f) * pivot_scale, Vec3f(0.2f, 1.0f, 0.2f), 4);
	draw_line(scene_viewport, pivot_pos + Vec3f(0.0f, 0.0f, 0.25f) * pivot_scale, pivot_pos + Vec3f(0.25f, 0.0f, 0.25f) * pivot_scale, Vec3f(0.2f, 0.2f, 1.0f), 5);
	draw_line(scene_viewport, pivot_pos + Vec3f(0.0f, 0.0f, 0.25f) * pivot_scale, pivot_pos + Vec3f(0.0f, 0.25f, 0.25f) * pivot_scale, Vec3f(0.2f, 0.2f, 1.0f), 5);
}

void ScenePivotAction::draw_rotate_pivot(const SceneViewportPtr &scene_viewport, uicore::Vec3f pos)
{
	float pivot_scale = 0.02f;
	Vec3f pivot_pos = scene_viewport->camera()->position() + Vec3f::normalize(pos - scene_viewport->camera()->position()) * 0.2f;
	Vec3f eye_dir = pivot_pos - scene_viewport->camera()->position();

	float s[73], t[73];
	for (int i = 0; i < 73; i++)
	{
		float angle = ((i * 5) % 360) * PI / 180.0f;
		s[i] = std::cos(angle);
		t[i] = std::sin(angle);
	}

	for (int i = 0; i < 72; i++)
	{
		Vec3f rx0 = Vec3f(0.0f, 1.0f, 0.0f) * s[i] + Vec3f(0.0f, 0.0f, 1.0f) * t[i];
		Vec3f rx1 = Vec3f(0.0f, 1.0f, 0.0f) * s[i + 1] + Vec3f(0.0f, 0.0f, 1.0f) * t[i + 1];
		Vec3f ry0 = Vec3f(1.0f, 0.0f, 0.0f) * s[i] + Vec3f(0.0f, 0.0f, 1.0f) * t[i];
		Vec3f ry1 = Vec3f(1.0f, 0.0f, 0.0f) * s[i + 1] + Vec3f(0.0f, 0.0f, 1.0f) * t[i + 1];
		Vec3f rz0 = Vec3f(1.0f, 0.0f, 0.0f) * s[i] + Vec3f(0.0f, 1.0f, 0.0f) * t[i];
		Vec3f rz1 = Vec3f(1.0f, 0.0f, 0.0f) * s[i + 1] + Vec3f(0.0f, 1.0f, 0.0f) * t[i + 1];

		if (Vec3f::dot(rx0, eye_dir) < 0.0f)
			draw_line(scene_viewport, pivot_pos + rx0 * pivot_scale, pivot_pos + rx1 * pivot_scale, Vec3f(1.0f, 0.2f, 0.2f), 0);
		if (Vec3f::dot(ry0, eye_dir) < 0.0f)
			draw_line(scene_viewport, pivot_pos + ry0 * pivot_scale, pivot_pos + ry1 * pivot_scale, Vec3f(0.2f, 0.1f, 0.2f), 1);
		if (Vec3f::dot(rz0, eye_dir) < 0.0f)
			draw_line(scene_viewport, pivot_pos + rz0 * pivot_scale, pivot_pos + rz1 * pivot_scale, Vec3f(0.2f, 0.2f, 0.1f), 2);
	}
}

void ScenePivotAction::draw_scale_pivot(const SceneViewportPtr &scene_viewport, uicore::Vec3f pos)
{
	float pivot_scale = 0.02f;
	Vec3f pivot_pos = scene_viewport->camera()->position() + Vec3f::normalize(pos - scene_viewport->camera()->position()) * 0.2f;
	Vec3f eye_dir = pivot_pos - scene_viewport->camera()->position();
	float dx = (Vec3f::dot(Vec3f(1.0f, 0.0f, 0.0f), eye_dir) < 0.0f) ? 1.0f : -1.0f;
	float dy = (Vec3f::dot(Vec3f(0.0f, 1.0f, 0.0f), eye_dir) < 0.0f) ? 1.0f : -1.0f;
	float dz = (Vec3f::dot(Vec3f(0.0f, 0.0f, 1.0f), eye_dir) < 0.0f) ? 1.0f : -1.0f;
	Vec3f dv(dx, dy, dz);
	draw_line(scene_viewport, pivot_pos + Vec3f(0.0f, 0.0f, 0.0f) * dv * pivot_scale, pivot_pos + Vec3f(0.8f, 0.0f, 0.0f) * dv * pivot_scale, Vec3f(1.0f, 0.2f, 0.2f), 0);
	draw_line(scene_viewport, pivot_pos + Vec3f(0.0f, 0.0f, 0.0f) * dv * pivot_scale, pivot_pos + Vec3f(0.0f, 0.8f, 0.0f) * dv * pivot_scale, Vec3f(0.2f, 1.0f, 0.2f), 1);
	draw_line(scene_viewport, pivot_pos + Vec3f(0.0f, 0.0f, 0.0f) * dv * pivot_scale, pivot_pos + Vec3f(0.0f, 0.0f, 0.8f) * dv * pivot_scale, Vec3f(0.2f, 0.2f, 1.0f), 2);
	draw_line(scene_viewport, pivot_pos + Vec3f(0.25f, 0.0f, 0.0f) * dv * pivot_scale, pivot_pos + Vec3f(0.0f, 0.0f, 0.25f) * dv * pivot_scale, Vec3f(1.0f, 1.0f, 0.2f), 3);
	draw_line(scene_viewport, pivot_pos + Vec3f(0.0f, 0.0f, 0.25f) * dv * pivot_scale, pivot_pos + Vec3f(0.0f, 0.25f, 0.0f) * dv * pivot_scale, Vec3f(1.0f, 1.0f, 0.2f), 3);
	draw_line(scene_viewport, pivot_pos + Vec3f(0.0f, 0.25f, 0.0f) * dv * pivot_scale, pivot_pos + Vec3f(0.25f, 0.0f, 0.0f) * dv * pivot_scale, Vec3f(1.0f, 1.0f, 0.2f), 3);
	draw_line(scene_viewport, pivot_pos + Vec3f(0.4f, 0.0f, 0.0f) * dv * pivot_scale, pivot_pos + Vec3f(0.0f, 0.0f, 0.4f) * dv * pivot_scale, Vec3f(1.0f, 1.0f, 0.2f), 3);
	draw_line(scene_viewport, pivot_pos + Vec3f(0.0f, 0.0f, 0.4f) * dv * pivot_scale, pivot_pos + Vec3f(0.0f, 0.4f, 0.0f) * dv * pivot_scale, Vec3f(1.0f, 1.0f, 0.2f), 3);
	draw_line(scene_viewport, pivot_pos + Vec3f(0.0f, 0.4f, 0.0f) * dv * pivot_scale, pivot_pos + Vec3f(0.4f, 0.0f, 0.0f) * dv * pivot_scale, Vec3f(1.0f, 1.0f, 0.2f), 3);
}
