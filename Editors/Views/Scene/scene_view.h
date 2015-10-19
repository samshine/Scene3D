
#pragma once

#include <scene3d.h>
#include "mouse_movement.h"

class SceneView : public uicore::ColumnView
{
public:
	SceneView();
	void render_content(const uicore::CanvasPtr &canvas) override;

	const ScenePtr &get_scene() { return scene; }

	uicore::Signal<void(const ScenePtr &, const uicore::GraphicContextPtr &, const uicore::DisplayWindowPtr &, const uicore::Vec2i &)> sig_update_scene;

private:
	void pointer_press(uicore::PointerEvent &e);
	void pointer_release(uicore::PointerEvent &e);
	void pointer_move(uicore::PointerEvent &e);

	void setup_scene(const uicore::GraphicContextPtr &gc);

	uicore::TimerPtr timer;
	MouseMovement mouse_movement;
	uicore::Pointf mouse_down_pos;
	bool mouse_down = false;
	uicore::Point last_mouse_movement;

	uicore::Texture2DPtr scene_texture;
	uicore::FrameBufferPtr scene_frame_buffer;

	SceneEnginePtr cache;
	ScenePtr scene;
};
