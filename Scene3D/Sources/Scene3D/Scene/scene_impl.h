
#pragma once

#include "Scene3D/scene.h"
#include "Scene3D/scene_engine.h"
#include "Scene3D/scene_camera.h"
#include "Scene3D/SceneEngine/scene_engine_impl.h"
#include "Scene3D/Culling/scene_cull_provider.h"
#include <list>

class SceneEngine;
class SceneEngineImpl;
typedef std::shared_ptr<SceneEngine> SceneEnginePtr;
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
	SceneImpl(const SceneEnginePtr &engine);

	void set_cull_oct_tree(const uicore::AxisAlignedBoundingBox &aabb) override;
	void set_cull_oct_tree(const uicore::Vec3f &aabb_min, const uicore::Vec3f &aabb_max) override;
	void set_cull_oct_tree(float max_size) override;

	void show_skybox_stars(bool enable) override;
	void set_skybox_gradient(std::vector<uicore::Colorf> &colors) override;

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

	SceneEngineImpl *engine() const { return _engine.get(); }

	std::vector<uicore::Colorf> skybox_gradient;

private:
	std::shared_ptr<SceneEngineImpl> _engine;

	std::list<SceneObjectImpl *> objects;
	std::list<SceneLightImpl *> lights;
	std::list<SceneLightProbeImpl *> light_probes;
	std::list<SceneParticleEmitterImpl *> emitters;

	std::unique_ptr<SceneCullProvider> cull_provider;

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
