
#include "precomp.h"
#include "Scene3D/scene.h"
#include "Scene3D/scene_object.h"
#include "Scene3D/scene_light.h"
#include "Scene3D/scene_particle_emitter.h"
#include "Scene3D/Performance/scope_timer.h"
#include "Scene3D/Culling/OctTree/oct_tree.h"
#include "scene_impl.h"
#include "scene_object_impl.h"
#include "scene_light_probe_impl.h"

using namespace uicore;

std::shared_ptr<Scene> Scene::create(const SceneEnginePtr &engine)
{
	return std::make_shared<SceneImpl>(engine);
}

SceneImpl::SceneImpl(const SceneEnginePtr &engine) : _engine(std::dynamic_pointer_cast<SceneEngineImpl>(engine))
{
	cull_provider = std::unique_ptr<SceneCullProvider>(new OctTree());
}

void SceneImpl::set_cull_oct_tree(const AxisAlignedBoundingBox &aabb)
{
	cull_provider = std::unique_ptr<SceneCullProvider>(new OctTree(aabb));
}

void SceneImpl::set_cull_oct_tree(const Vec3f &aabb_min, const Vec3f &aabb_max)
{
	if (!objects.empty() || !lights.empty() || !emitters.empty() || !light_probes.empty())
		throw Exception("Cannot change scene culling strategy after objects have been added");

	set_cull_oct_tree(AxisAlignedBoundingBox(aabb_min, aabb_max));
}

void SceneImpl::set_cull_oct_tree(float max_size)
{
	set_cull_oct_tree(AxisAlignedBoundingBox(Vec3f(-max_size), Vec3f(max_size)));
}

void SceneImpl::show_skybox_stars(bool enable)
{
	engine()->render.show_skybox_stars = enable;
}

void SceneImpl::set_skybox_gradient(std::vector<Colorf> &colors)
{
	skybox_gradient = colors;
	engine()->render.skybox_texture = nullptr;
}

void SceneImpl::foreach(const FrustumPlanes &frustum, const std::function<void(SceneItem *)> &callback)
{
	ScopeTimeFunction();
	for (SceneItem *item : cull_provider->cull(frustum))
		callback(item);
}

void SceneImpl::foreach(const Vec3f &position, const std::function<void(SceneItem *)> &callback)
{
	ScopeTimeFunction();
	for (SceneItem *item : cull_provider->cull(position))
		callback(item);
}

void SceneImpl::foreach_object(const FrustumPlanes &frustum, const std::function<void(SceneObjectImpl *)> &callback)
{
	ScopeTimeFunction();
	for (SceneItem *item : cull_provider->cull(frustum))
	{
		SceneObjectImpl *v = dynamic_cast<SceneObjectImpl*>(item);
		if (v)
			callback(v);
	}
}

void SceneImpl::foreach_object(const Vec3f &position, const std::function<void(SceneObjectImpl *)> &callback)
{
	ScopeTimeFunction();
	for (SceneItem *item : cull_provider->cull(position))
	{
		SceneObjectImpl *v = dynamic_cast<SceneObjectImpl*>(item);
		if (v)
			callback(v);
	}
}

void SceneImpl::foreach_light(const FrustumPlanes &frustum, const std::function<void(SceneLightImpl *)> &callback)
{
	ScopeTimeFunction();
	for (SceneItem *item : cull_provider->cull(frustum))
	{
		SceneLightImpl *v = dynamic_cast<SceneLightImpl*>(item);
		if (v)
			callback(v);
	}
}

void SceneImpl::foreach_light(const Vec3f &position, const std::function<void(SceneLightImpl *)> &callback)
{
	ScopeTimeFunction();
	for (SceneItem *item : cull_provider->cull(position))
	{
		SceneLightImpl *v = dynamic_cast<SceneLightImpl*>(item);
		if (v)
			callback(v);
	}
}

void SceneImpl::foreach_light_probe(const FrustumPlanes &frustum, const std::function<void(SceneLightProbeImpl *)> &callback)
{
	ScopeTimeFunction();
	for (SceneItem *item : cull_provider->cull(frustum))
	{
		SceneLightProbeImpl *v = dynamic_cast<SceneLightProbeImpl*>(item);
		if (v)
			callback(v);
	}
}

void SceneImpl::foreach_light_probe(const Vec3f &position, const std::function<void(SceneLightProbeImpl *)> &callback)
{
	ScopeTimeFunction();
	for (SceneItem *item : cull_provider->cull(position))
	{
		SceneLightProbeImpl *v = dynamic_cast<SceneLightProbeImpl*>(item);
		if (v)
			callback(v);
	}
}

void SceneImpl::foreach_emitter(const FrustumPlanes &frustum, const std::function<void(SceneParticleEmitterImpl *)> &callback)
{
	ScopeTimeFunction();
	for (SceneItem *item : cull_provider->cull(frustum))
	{
		SceneParticleEmitterImpl *v = dynamic_cast<SceneParticleEmitterImpl*>(item);
		if (v)
			callback(v);
	}
}

void SceneImpl::foreach_emitter(const Vec3f &position, const std::function<void(SceneParticleEmitterImpl *)> &callback)
{
	ScopeTimeFunction();
	for (SceneItem *item : cull_provider->cull(position))
	{
		SceneParticleEmitterImpl *v = dynamic_cast<SceneParticleEmitterImpl*>(item);
		if (v)
			callback(v);
	}
}
