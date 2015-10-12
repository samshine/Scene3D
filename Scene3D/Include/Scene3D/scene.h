
#pragma once

#include "Performance/gpu_timer.h"
#include <memory>

class SceneCache;
typedef std::shared_ptr<SceneCache> SceneCachePtr;
class Scene_Impl;
class SceneLight;
class SceneParticleEmitter;
class SceneObject;
class SceneCamera;
class ScenePass;

class Scene
{
public:
	Scene();
	Scene(const SceneCachePtr &cache);

	bool is_null() const;

	const SceneCamera &get_camera() const;
	SceneCamera &get_camera();

	void set_viewport(const uicore::Rect &box, const uicore::FrameBufferPtr &fb = nullptr);
	void set_camera(const SceneCamera &camera);

	void render(const uicore::GraphicContextPtr &gc);

	void update(const uicore::GraphicContextPtr &gc, float time_elapsed);

	uicore::Mat4f world_to_eye() const;
	uicore::Mat4f eye_to_projection() const;
	uicore::Mat4f world_to_projection() const;

	void unproject(const uicore::Vec2i &screen_pos, uicore::Vec3f &out_ray_start, uicore::Vec3f &out_ray_direction);

	void set_cull_oct_tree(const uicore::AxisAlignedBoundingBox &aabb);
	void set_cull_oct_tree(const uicore::Vec3f &aabb_min, const uicore::Vec3f &aabb_max);
	void set_cull_oct_tree(float max_size);

	ScenePass add_pass(const std::string &name, const std::string &insert_before = std::string());
	void remove_pass(const std::string &name);

	void show_skybox_stars(bool enable);
	void set_skybox_gradient(const uicore::GraphicContextPtr &gc, std::vector<uicore::Colorf> &colors);

	int models_drawn() const;
	int instances_drawn() const;
	int draw_calls() const;
	int triangles_drawn() const;
	int scene_visits() const;
	const std::vector<GPUTimer::Result> &gpu_results() const;

private:
	std::shared_ptr<Scene_Impl> impl;

	friend class SceneLight;
	friend class SceneParticleEmitter;
	friend class SceneObject;
	friend class SceneCamera;
	friend class SceneModel;
	friend class SceneLightProbe;
	friend class ScenePass;
};
