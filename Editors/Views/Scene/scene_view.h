
#pragma once

#include <scene3d.h>
#include "mouse_movement.h"

class SceneView : public uicore::View
{
public:
	SceneView();
	void render_content(uicore::Canvas &canvas) override;

	uicore::Scene &get_scene() { return scene; }

	uicore::Signal<void(uicore::Scene &, uicore::GraphicContext &, uicore::DisplayWindow &, const uicore::Vec2i &)> sig_update_scene;

private:
	void pointer_press(uicore::PointerEvent &e);
	void pointer_release(uicore::PointerEvent &e);
	void pointer_move(uicore::PointerEvent &e);

	void setup_scene(uicore::GraphicContext &gc);

	uicore::Timer timer;
	MouseMovement mouse_movement;
	uicore::Pointf mouse_down_pos;
	bool mouse_down = false;
	uicore::Point last_mouse_movement;

	uicore::Texture2D scene_texture;
	uicore::FrameBuffer scene_frame_buffer;

	uicore::SceneCache cache;
	uicore::Scene scene;
};
