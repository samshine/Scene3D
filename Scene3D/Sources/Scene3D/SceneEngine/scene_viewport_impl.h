
#pragma once

#include "Scene3D/scene_viewport.h"
#include "Scene3D/scene_engine.h"
#include "Scene3D/scene_camera.h"
#include "Scene3D/SceneEngine/scene_engine_impl.h"

class SceneImpl;
class SceneEngineImpl;
typedef std::shared_ptr<SceneEngine> SceneEnginePtr;

class SceneLines
{
public:
	std::vector<uicore::Vec3f> points;
	std::vector<uicore::Vec3f> colors;
	std::vector<uicore::Vec3f> text_locations;
	std::vector<size_t> text_offsets;
	std::vector<size_t> text_lengths;
	std::string text;
};

class SceneViewportImpl : public SceneViewport
{
public:
	SceneViewportImpl(const SceneEnginePtr &engine);

	const SceneCameraPtr &camera() const override { return _camera; }
	void set_camera(const SceneCameraPtr &camera) override { _camera = camera; }

	void clear_lines() override;
	void draw_line(const uicore::Vec3f &from, const uicore::Vec3f &to, const uicore::Vec3f &color) override;
	void draw_3d_text(const uicore::Vec3f &location, const char *text) override;

	void set_viewport(const uicore::Rect &box, const uicore::FrameBufferPtr &fb) override;
	void render(const uicore::GraphicContextPtr &gc) override;

	void update(const uicore::GraphicContextPtr &gc, float time_elapsed) override;

	uicore::Mat4f world_to_eye() const override;
	uicore::Mat4f eye_to_projection() const override;
	uicore::Mat4f world_to_projection() const override;

	void unproject(const uicore::Vec2i &screen_pos, uicore::Vec3f &out_ray_start, uicore::Vec3f &out_ray_direction) override;

	SceneImpl *scene() const;
	SceneEngineImpl *engine() const { return _engine.get(); }

	uicore::Rect _viewport;
	uicore::FrameBufferPtr _fb_viewport;

	SceneLines _scene_lines;

private:
	std::shared_ptr<SceneEngineImpl> _engine;
	SceneCameraPtr _camera;
};
