
#pragma once

#include <scene3d.h>
#include "mouse_movement.h"

class SceneView : public uicore::ColumnView
{
public:
	SceneView();
	void render_content(const uicore::CanvasPtr &canvas) override;

	const SceneViewportPtr &viewport() { return scene_viewport; }

	uicore::Signal<void(const SceneViewportPtr &, const uicore::GraphicContextPtr &, const uicore::DisplayWindowPtr &, const uicore::Vec2i &)> sig_update_scene;

	static SceneEnginePtr engine();

private:
	void pointer_press(uicore::PointerEvent &e);
	void pointer_release(uicore::PointerEvent &e);
	void pointer_move(uicore::PointerEvent &e);

	uicore::TimerPtr timer;
	uicore::Pointf mouse_down_pos;
	bool mouse_down = false;
	uicore::Point last_mouse_movement;

	uicore::Texture2DPtr scene_texture;
	uicore::FrameBufferPtr scene_frame_buffer;

	SceneViewportPtr scene_viewport = SceneViewport::create(engine());
};
