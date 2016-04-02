
#include "precomp.h"
#include "scene_rotate_action.h"
#include "mouse_movement.h"

using namespace uicore;

void SceneRotateAction::pointer_press(PointerEvent *e)
{
	if (e->button() == PointerButton::right)
	{
		begin_action();

		view()->view_tree()->display_window()->hide_cursor();
		mouse_down_pos = view()->view_tree()->display_window()->mouse()->position();
		last_mouse_movement = MouseMovement::instance().pos();
	}
}

void SceneRotateAction::pointer_release(PointerEvent *e)
{
	if (!action_active())
		return;

	if (e->button() == PointerButton::right)
	{
		end_action();

		view()->view_tree()->display_window()->mouse()->set_position(mouse_down_pos.x, mouse_down_pos.y);
		view()->view_tree()->display_window()->show_cursor();
	}
}

void SceneRotateAction::pointer_move(PointerEvent *e)
{
	if (!action_active())
		return;

	Point move = MouseMovement::instance().pos();
	Vec2i delta_mouse_move = move - last_mouse_movement;
	last_mouse_movement = move;

	Sizef size = view()->view_tree()->display_window()->geometry().size();
	view()->view_tree()->display_window()->mouse()->set_position(size.width * 0.5f, size.height * 0.5f);

	func_mouse_move(delta_mouse_move);
}
