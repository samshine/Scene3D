
#pragma once

#include <memory>

class SceneEngine;
typedef std::shared_ptr<SceneEngine> SceneEnginePtr;
class SceneCamera;
typedef std::shared_ptr<SceneCamera> SceneCameraPtr;

class Scene
{
public:
	static std::shared_ptr<Scene> create(const SceneEnginePtr &engine);

	virtual const SceneCameraPtr &camera() const = 0;

	virtual void set_viewport(const uicore::Rect &box, const uicore::FrameBufferPtr &fb = nullptr) = 0;
	virtual void set_camera(const SceneCameraPtr &camera) = 0;

	virtual void render(const uicore::GraphicContextPtr &gc) = 0;

	virtual void update(const uicore::GraphicContextPtr &gc, float time_elapsed) = 0;

	virtual uicore::Mat4f world_to_eye() const = 0;
	virtual uicore::Mat4f eye_to_projection() const = 0;
	virtual uicore::Mat4f world_to_projection() const = 0;

	virtual void unproject(const uicore::Vec2i &screen_pos, uicore::Vec3f &out_ray_start, uicore::Vec3f &out_ray_direction) = 0;

	virtual void set_cull_oct_tree(const uicore::AxisAlignedBoundingBox &aabb) = 0;
	virtual void set_cull_oct_tree(const uicore::Vec3f &aabb_min, const uicore::Vec3f &aabb_max) = 0;
	virtual void set_cull_oct_tree(float max_size) = 0;

	virtual void show_skybox_stars(bool enable) = 0;
	virtual void set_skybox_gradient(const uicore::GraphicContextPtr &gc, std::vector<uicore::Colorf> &colors) = 0;
};

typedef std::shared_ptr<Scene> ScenePtr;
