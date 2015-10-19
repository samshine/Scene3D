
#pragma once

#include "Scene3D/scene.h"
#include "Scene3D/scene_cache.h"
#include "Scene3D/scene_camera.h"
#include "Scene3D/SceneCache/scene_cache_impl.h"
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

	void set_camera(const uicore::Vec3f &position, const uicore::Quaternionf &orientation);
	void set_camera_position(const uicore::Vec3f &position);
	void set_camera_orientation(const uicore::Quaternionf &orientation);

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

	SceneCacheImpl *get_cache() const { return cache.get(); }

private:
	std::shared_ptr<SceneCacheImpl> cache;

	std::list<SceneObjectImpl *> objects;
	std::list<SceneLightImpl *> lights;
	std::list<SceneLightProbeImpl *> light_probes;
	std::list<SceneParticleEmitterImpl *> emitters;

	std::unique_ptr<SceneCullProvider> cull_provider;

	SceneCameraPtr _camera;

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
