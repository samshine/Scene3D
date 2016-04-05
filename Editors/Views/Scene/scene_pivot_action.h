
#pragma once

#include "scene_view.h"

enum class ScenePivotType
{
	none,
	move,
	rotate,
	scale
};

class ScenePivotAction : public SceneViewAction
{
public:
	ScenePivotType type() const { return _pivot_type; }
	uicore::Vec3f position() const { return _pivot_pos; }
	uicore::Quaternionf rotation() const { return _pivot_rotation; }
	uicore::Vec3f scale() const { return _pivot_scale; }

	void set_position(const uicore::Vec3f &pos) { _pivot_pos = pos; view()->set_needs_render(); }
	void set_rotation(const uicore::Quaternionf &rotate) { _pivot_rotation = rotate; view()->set_needs_render(); }
	void set_scale(const uicore::Vec3f &scale) { _pivot_scale = scale; view()->set_needs_render(); }
	void set_type(ScenePivotType type) { _pivot_type = type; view()->set_needs_render(); }

	uicore::Signal<void()> &sig_pivot_changed() { return _sig_pivot_changed; }

	void pointer_press(uicore::PointerEvent *e) override;
	void pointer_double_click(uicore::PointerEvent *e) override;
	void pointer_release(uicore::PointerEvent *e) override;
	void pointer_move(uicore::PointerEvent *e) override;
	void draw(const SceneViewportPtr &viewport) override;

private:
	void draw_move_pivot(const SceneViewportPtr &viewport, uicore::Vec3f pos);
	void draw_rotate_pivot(const SceneViewportPtr &viewport, uicore::Vec3f pos);
	void draw_scale_pivot(const SceneViewportPtr &viewport, uicore::Vec3f pos);

	int hit_test(const uicore::Vec2f &pos);
	void draw_line(const SceneViewportPtr &viewport, uicore::Vec3f from, uicore::Vec3f to, uicore::Vec3f color, int hit_type);

	std::vector<std::pair<uicore::LineSegment2f, int>> lines;

	ScenePivotType _pivot_type = ScenePivotType::none;
	uicore::Vec3f _pivot_pos;
	uicore::Quaternionf _pivot_rotation;
	uicore::Vec3f _pivot_scale = uicore::Vec3f(1.0f);
	uicore::Signal<void()> _sig_pivot_changed;

	int hit_mode = -1;
};
