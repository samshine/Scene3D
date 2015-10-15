
#pragma once

#include "Scene3D/scene.h"
#include "Scene3D/scene_cache.h"
#include "Scene3D/scene_camera.h"
#include "Scene3D/Performance/gpu_timer.h"
#include "Scene3D/SceneCache/material_cache.h"
#include "Scene3D/SceneCache/instances_buffer.h"
#include "Scene3D/SceneCache/scene_cache_impl.h"
#include "Scene3D/SceneCache/resource_container.h"
#include "Scene3D/SceneCache/resource.h"
#include "Scene3D/Model/model_shader_cache.h"
#include "Scene3D/Model/model_cache.h"
#include "Scene3D/Passes/VSMShadowMap/vsm_shadow_map_pass.h"
#include "Scene3D/Passes/Lightsource/lightsource_pass.h"
#include "Scene3D/Passes/LightsourceSimple/lightsource_simple_pass.h"
#include "Scene3D/Passes/GBuffer/gbuffer_pass.h"
#include "Scene3D/Passes/Skybox/skybox_pass.h"
#include "Scene3D/Passes/Bloom/bloom_pass.h"
#include "Scene3D/Passes/SSAO/ssao_pass.h"
#include "Scene3D/Passes/Final/final_pass.h"
#include "Scene3D/Passes/Transparency/transparency_pass.h"
#include "Scene3D/Passes/ParticleEmitter/particle_emitter_pass.h"
#include "Scene3D/Culling/scene_cull_provider.h"
#include <list>

class SceneCache;
class SceneCacheImpl;
typedef std::shared_ptr<SceneCache> SceneCachePtr;
class ModelMeshVisitor;
class SceneObject_Impl;
class SceneLight_Impl;
class SceneLightVisitor;
class Resource_BaseImpl;
class SceneParticleEmitter_Impl;
class SceneParticleEmitterVisitor;
class SceneLightProbe_Impl;

class Scene_Impl : public Scene
{
public:
	Scene_Impl(const SceneCachePtr &cache);

	const SceneCameraPtr &camera() const override { return _camera; }
	void set_camera(const SceneCameraPtr &camera) override { _camera = camera; }

	void set_viewport(const uicore::Rect &box, const uicore::FrameBufferPtr &fb) override;
	void render(const uicore::GraphicContextPtr &gc) override;

	void update(const uicore::GraphicContextPtr &gc, float time_elapsed) override;

	uicore::Mat4f world_to_eye() const override;
	uicore::Mat4f eye_to_projection() const override;
	uicore::Mat4f world_to_projection() const override;

	void unproject(const uicore::Vec2i &screen_pos, uicore::Vec3f &out_ray_start, uicore::Vec3f &out_ray_direction) override;

	void set_cull_oct_tree(const uicore::AxisAlignedBoundingBox &aabb) override;
	void set_cull_oct_tree(const uicore::Vec3f &aabb_min, const uicore::Vec3f &aabb_max) override;
	void set_cull_oct_tree(float max_size) override;

	void show_skybox_stars(bool enable) override;
	void set_skybox_gradient(const uicore::GraphicContextPtr &gc, std::vector<uicore::Colorf> &colors) override;

	int models_drawn() const override { return _models_drawn; }
	int instances_drawn() const override { return _instances_drawn; }
	int draw_calls() const override { return _draw_calls; }
	int triangles_drawn() const override { return _triangles_drawn; }
	int scene_visits() const override { return _scene_visits; }
	const std::vector<GPUTimer::Result> &gpu_results() const override { return _gpu_results; }

	void set_camera(const uicore::Vec3f &position, const uicore::Quaternionf &orientation);
	void set_camera_position(const uicore::Vec3f &position);
	void set_camera_orientation(const uicore::Quaternionf &orientation);
	void set_camera_field_of_view(float fov) { camera_field_of_view.set(fov); }

	void visit(const uicore::GraphicContextPtr &gc, const uicore::Mat4f &world_to_eye, const uicore::Mat4f &eye_to_projection, uicore::FrustumPlanes frustum, ModelMeshVisitor *visitor);
	void visit_lights(const uicore::GraphicContextPtr &gc, const uicore::Mat4f &world_to_eye, const uicore::Mat4f &eye_to_projection, uicore::FrustumPlanes frustum, SceneLightVisitor *visitor);
	void visit_emitters(const uicore::GraphicContextPtr &gc, const uicore::Mat4f &world_to_eye, const uicore::Mat4f &eye_to_projection, uicore::FrustumPlanes frustum, SceneParticleEmitterVisitor *visitor);

	SceneLightProbe_Impl *find_nearest_probe(const uicore::Vec3f &position);

	GPUTimer &get_gpu_timer() { return gpu_timer; }

	SceneCacheImpl *get_cache() const { return cache.get(); }

	ResourceContainer inout_data;
	std::vector<std::shared_ptr<ScenePass>> passes;

	int _models_drawn = 0;
	int _instances_drawn = 0;
	int _draw_calls = 0;
	int _triangles_drawn = 0;
	int _scene_visits = 0;
	std::vector<GPUTimer::Result> _gpu_results;

private:
	std::shared_ptr<SceneCacheImpl> cache;

	int frame = 0;
	InstancesBuffer instances_buffer;

	std::unique_ptr<MaterialCache> material_cache;
	std::unique_ptr<ModelShaderCache> model_shader_cache;
	std::unique_ptr<ModelCache> model_cache;

	std::list<SceneObject_Impl *> objects;
	std::list<SceneLight_Impl *> lights;
	std::list<SceneLightProbe_Impl *> light_probes;
	std::list<SceneParticleEmitter_Impl *> emitters;

	std::unique_ptr<SceneCullProvider> cull_provider;

	SceneCameraPtr _camera;

	Resource<float> camera_field_of_view;
	Resource<uicore::FrameBufferPtr> viewport_fb;
	Resource<uicore::Rect> viewport;
	Resource<uicore::Mat4f> out_world_to_eye;
	Resource<uicore::Texture2DPtr> skybox_texture;
	Resource<bool> _show_skybox_stars;

	GPUTimer gpu_timer;

	friend class SceneObject;
	friend class SceneObject_Impl;
	friend class SceneLight;
	friend class SceneLight_Impl;
	friend class SceneLightProbe;
	friend class SceneLightProbe_Impl;
	friend class SceneParticleEmitter;
	friend class SceneParticleEmitter_Impl;
	friend class SceneModel;
	friend class SceneModel_Impl;
	friend class Scene;
};
