
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
#include "Scene3D/Passes/LensFlare/lens_flare_pass.h"
#include "Scene3D/Culling/scene_cull_provider.h"
#include <list>

class SceneCache;
class SceneCacheImpl;
typedef std::shared_ptr<SceneCache> SceneCachePtr;
class ModelMeshVisitor;
class SceneObjectImpl;
class SceneLightImpl;
class SceneLightVisitor;
class Resource_BaseImpl;
class SceneParticleEmitterImpl;
class SceneParticleEmitterVisitor;
class SceneLightProbeImpl;

class SceneImpl : public Scene
{
public:
	SceneImpl(const SceneCachePtr &cache);

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
	void set_camera_field_of_view(float fov) { inout_data.field_of_view = fov; }

	void foreach(const uicore::FrustumPlanes &frustum, const std::function<void(SceneItem *)> &callback);
	void foreach(const uicore::Vec3f &position, const std::function<void(SceneItem *)> &callback);
	void foreach_object(const uicore::FrustumPlanes &frustum, const std::function<void(SceneObjectImpl *)> &callback);
	void foreach_object(const uicore::Vec3f &position, const std::function<void(SceneObjectImpl *)> &callback);
	void foreach_light(const uicore::FrustumPlanes &frustum, const std::function<void(SceneLightImpl *)> &callback);
	void foreach_light(const uicore::Vec3f &position, const std::function<void(SceneLightImpl *)> &callback);
	void foreach_light_probe(const uicore::FrustumPlanes &frustum, const std::function<void(SceneLightProbeImpl *)> &callback);
	void foreach_light_probe(const uicore::Vec3f &position, const std::function<void(SceneLightProbeImpl *)> &callback);
	void foreach_emitter(const uicore::FrustumPlanes &frustum, const std::function<void(SceneParticleEmitterImpl *)> &callback);
	void foreach_emitter(const uicore::Vec3f &position, const std::function<void(SceneParticleEmitterImpl *)> &callback);

	template<typename T> void foreach_type(const uicore::FrustumPlanes &frustum, const std::function<void(T *)> &callback)
	{
		foreach(frustum, [&](SceneItem *item)
		{
			T *v = dynamic_cast<T*>(item);
			if (v)
				callback(v);
		});
	}

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

	InstancesBuffer instances_buffer;

private:
	std::shared_ptr<SceneCacheImpl> cache;

	std::unique_ptr<MaterialCache> material_cache;
	std::unique_ptr<ModelShaderCache> model_shader_cache;
	std::unique_ptr<ModelCache> model_cache;

	std::list<SceneObjectImpl *> objects;
	std::list<SceneLightImpl *> lights;
	std::list<SceneLightProbeImpl *> light_probes;
	std::list<SceneParticleEmitterImpl *> emitters;

	std::unique_ptr<SceneCullProvider> cull_provider;

	SceneCameraPtr _camera;

	GPUTimer gpu_timer;

	friend class SceneObject;
	friend class SceneObjectImpl;
	friend class SceneLight;
	friend class SceneLightImpl;
	friend class SceneLightProbe;
	friend class SceneLightProbeImpl;
	friend class SceneParticleEmitter;
	friend class SceneParticleEmitterImpl;
	friend class SceneModel;
	friend class SceneModelImpl;
	friend class Scene;
};
