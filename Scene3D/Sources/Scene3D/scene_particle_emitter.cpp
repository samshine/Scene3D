
#include "precomp.h"
#include "Scene3D/scene_particle_emitter.h"
#include "Scene3D/scene.h"
#include "Passes/ParticleEmitter/particle_emitter_pass_data.h"
#include "Scene3D/scene_particle_emitter_impl.h"
#include "Scene3D/scene_impl.h"

using namespace uicore;

std::shared_ptr<SceneParticleEmitter> SceneParticleEmitter::create(Scene &scene)
{
	return std::make_shared<SceneParticleEmitter_Impl>(scene.impl.get());
}

SceneParticleEmitter_Impl::SceneParticleEmitter_Impl(Scene_Impl *scene) : scene(scene)
{
	it = scene->emitters.insert(scene->emitters.end(), this);

	cull_proxy = scene->cull_provider->create_proxy(this, get_aabb());
}

SceneParticleEmitter_Impl::~SceneParticleEmitter_Impl()
{
	if (cull_proxy)
		scene->cull_provider->delete_proxy(cull_proxy);
	scene->emitters.erase(it);

	if (pass_data)
	{
		pass_data->visible = false;
		pass_data->emitter = 0;
	}
}

void SceneParticleEmitter_Impl::set_position(const Vec3f &position)
{
	if (_position != position)
	{
		_position = position;
		if (cull_proxy)
			scene->cull_provider->set_aabb(cull_proxy, get_aabb());
	}
}

void SceneParticleEmitter_Impl::set_orientation(const Quaternionf &orientation)
{
	_orientation = orientation;
}

AxisAlignedBoundingBox SceneParticleEmitter_Impl::get_aabb()
{
	float attenuation_end = 30.0f;
	AxisAlignedBoundingBox aabb;
	aabb.aabb_min = _position - attenuation_end * 1.73205081f;
	aabb.aabb_max = _position + attenuation_end * 1.73205081f;
	return aabb;
}
