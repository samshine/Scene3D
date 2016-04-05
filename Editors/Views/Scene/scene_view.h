
#pragma once

#include <scene3d.h>

class SceneViewAction : public uicore::ViewAction
{
public:
	virtual void draw(const SceneViewportPtr &viewport) { }
};

class SceneView : public uicore::ColumnView
{
public:
	SceneView();

	static SceneEnginePtr engine();

	const SceneViewportPtr &viewport() { return scene_viewport; }

	void set_animate(bool value = true);
	void unproject(const uicore::Vec2i &pos, uicore::Vec3f &out_ray_start, uicore::Vec3f &out_ray_direction);

	uicore::Signal<void(const SceneViewportPtr &, const uicore::GraphicContextPtr &, const uicore::DisplayWindowPtr &)> &sig_update_scene() { return _sig_update_scene; }

	void render_content(const uicore::CanvasPtr &canvas) override;

private:
	void pointer_press(uicore::PointerEvent *e);

	void draw_bbox(uicore::Vec3f bbox_min, uicore::Vec3f bbox_max);
	void draw_grid();

	uicore::TimerPtr timer;

	uicore::Texture2DPtr scene_texture;
	uicore::FrameBufferPtr scene_frame_buffer;

	SceneViewportPtr scene_viewport = SceneViewport::create(engine());

	uicore::Signal<void(const SceneViewportPtr &, const uicore::GraphicContextPtr &, const uicore::DisplayWindowPtr &)> _sig_update_scene;
};
