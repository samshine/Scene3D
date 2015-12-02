
#pragma once

class SceneRotateAction : public uicore::ViewAction
{
public:
	void pointer_press(uicore::PointerEvent &e) override;
	void pointer_release(uicore::PointerEvent &e) override;
	void pointer_move(uicore::PointerEvent &e) override;

	std::function<void(const uicore::Vec2i &delta)> func_mouse_move;

private:
	uicore::Pointf mouse_down_pos;
	uicore::Point last_mouse_movement;
};
