
#pragma once

#include <memory>

class SceneEngine;
typedef std::shared_ptr<SceneEngine> SceneEnginePtr;
class SceneCamera;
typedef std::shared_ptr<SceneCamera> SceneCameraPtr;

class SceneViewport
{
public:
	static std::shared_ptr<SceneViewport> create(const SceneEnginePtr &engine);

	virtual const SceneCameraPtr &camera() const = 0;
	virtual void set_camera(const SceneCameraPtr &camera) = 0;

	virtual void clear_lines() = 0;
	virtual void draw_line(const uicore::Vec3f &from, const uicore::Vec3f &to, const uicore::Vec3f &color) = 0;
	virtual void draw_3d_text(const uicore::Vec3f &location, const char *text) = 0;

	virtual void set_viewport(const uicore::Rect &box, const uicore::FrameBufferPtr &fb = nullptr, float scale = 1.0f) = 0;
	virtual void render(const uicore::GraphicContextPtr &gc) = 0;
	virtual void update(const uicore::GraphicContextPtr &gc, float time_elapsed) = 0;

	virtual uicore::Mat4f world_to_eye() const = 0;
	virtual uicore::Mat4f eye_to_projection() const = 0;
	virtual uicore::Mat4f world_to_projection() const = 0;

	virtual void unproject(const uicore::Vec2i &screen_pos, uicore::Vec3f &out_ray_start, uicore::Vec3f &out_ray_direction) = 0;
};

typedef std::shared_ptr<SceneViewport> SceneViewportPtr;
