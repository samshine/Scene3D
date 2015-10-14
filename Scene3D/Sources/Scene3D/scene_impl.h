
#pragma once

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

class Scene_Impl
{
public:
	Scene_Impl(const SceneCachePtr &cache);

	void set_viewport(const uicore::Rect &box, const uicore::FrameBufferPtr &fb = nullptr);
	void set_camera(const uicore::Vec3f &position, const uicore::Quaternionf &orientation);
	void set_camera_position(const uicore::Vec3f &position);
	void set_camera_orientation(const uicore::Quaternionf &orientation);
	void set_camera_field_of_view(float fov) { camera_field_of_view.set(fov); }
	void render(const uicore::GraphicContextPtr &gc);
	void update(const uicore::GraphicContextPtr &gc, float time_elapsed);

	void visit(const uicore::GraphicContextPtr &gc, const uicore::Mat4f &world_to_eye, const uicore::Mat4f &eye_to_projection, uicore::FrustumPlanes frustum, ModelMeshVisitor *visitor);
	void visit_lights(const uicore::GraphicContextPtr &gc, const uicore::Mat4f &world_to_eye, const uicore::Mat4f &eye_to_projection, uicore::FrustumPlanes frustum, SceneLightVisitor *visitor);
	void visit_emitters(const uicore::GraphicContextPtr &gc, const uicore::Mat4f &world_to_eye, const uicore::Mat4f &eye_to_projection, uicore::FrustumPlanes frustum, SceneParticleEmitterVisitor *visitor);

	SceneLightProbe_Impl *find_nearest_probe(const uicore::Vec3f &position);

	GPUTimer &get_gpu_timer() { return gpu_timer; }

	SceneCacheImpl *get_cache() const { return cache.get(); }

	const SceneCameraPtr &get_camera() const { return camera; }
	void set_camera(const SceneCameraPtr &cam) { camera = cam; }

	ResourceContainer inout_data;
	std::vector<std::shared_ptr<ScenePass>> passes;

	int models_drawn = 0;
	int instances_drawn = 0;
	int draw_calls = 0;
	int triangles_drawn = 0;
	int scene_visits = 0;
	std::vector<GPUTimer::Result> gpu_results;

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

	SceneCameraPtr camera;

	Resource<float> camera_field_of_view;
	Resource<uicore::FrameBufferPtr> viewport_fb;
	Resource<uicore::Rect> viewport;
	Resource<uicore::Mat4f> out_world_to_eye;
	Resource<uicore::Texture2DPtr> skybox_texture;
	Resource<bool> show_skybox_stars;

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
