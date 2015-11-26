
#pragma once

#include "Scene3D/scene_viewport.h"
#include "Scene3D/scene_engine.h"
#include "Scene3D/scene_camera.h"
#include "Scene3D/SceneEngine/scene_engine_impl.h"

class SceneImpl;
class SceneEngineImpl;
typedef std::shared_ptr<SceneEngine> SceneEnginePtr;

class SceneViewportImpl : public SceneViewport
{
public:
	SceneViewportImpl(const SceneEnginePtr &engine);

	const SceneCameraPtr &camera() const override { return _camera; }
	void set_camera(const SceneCameraPtr &camera) override { _camera = camera; }

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

private:
	std::shared_ptr<SceneEngineImpl> _engine;
	SceneCameraPtr _camera;
};
