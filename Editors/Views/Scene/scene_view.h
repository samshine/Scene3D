
#pragma once

#include <scene3d.h>

class SceneView : public uicore::ColumnView
{
public:
	SceneView();
	void set_animate(bool value = true);
	const SceneViewportPtr &viewport() { return scene_viewport; }
	uicore::Signal<void(const SceneViewportPtr &, const uicore::GraphicContextPtr &, const uicore::DisplayWindowPtr &)> sig_update_scene;

	static SceneEnginePtr engine();

	void unproject(const uicore::Vec2i &pos, uicore::Vec3f &out_ray_start, uicore::Vec3f &out_ray_direction);
	void render_content(const uicore::CanvasPtr &canvas) override;

private:
	void pointer_press(uicore::PointerEvent *e);

	uicore::TimerPtr timer;

	uicore::Texture2DPtr scene_texture;
	uicore::FrameBufferPtr scene_frame_buffer;

	SceneViewportPtr scene_viewport = SceneViewport::create(engine());
};
