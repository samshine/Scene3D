
#pragma once

#include "Performance/gpu_timer.h"
#include <memory>

class SceneCache;
typedef std::shared_ptr<SceneCache> SceneCachePtr;
class SceneCamera;
typedef std::shared_ptr<SceneCamera> SceneCameraPtr;

class Scene
{
public:
	static std::shared_ptr<Scene> create(const SceneCachePtr &cache);

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

	virtual int models_drawn() const = 0;
	virtual int instances_drawn() const = 0;
	virtual int draw_calls() const = 0;
	virtual int triangles_drawn() const = 0;
	virtual int scene_visits() const = 0;
	virtual const std::vector<GPUTimer::Result> &gpu_results() const = 0;
};

typedef std::shared_ptr<Scene> ScenePtr;
