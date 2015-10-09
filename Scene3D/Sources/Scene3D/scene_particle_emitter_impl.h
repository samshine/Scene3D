
#pragma once

#include "Scene3D/scene_cull_provider.h"
#include <list>

class Scene_Impl;

class SceneParticleEmitter_Impl : public SceneItem
{
public:
	SceneParticleEmitter_Impl(Scene_Impl *scene);
	~SceneParticleEmitter_Impl();

	uicore::AxisAlignedBoundingBox get_aabb();

	Scene_Impl *scene;
	SceneCullProxy *cull_proxy;
	std::list<SceneParticleEmitter_Impl *>::iterator it;

	SceneParticleEmitter::Type type;
	uicore::Vec3f position;
	uicore::Quaternionf orientation;

	float particles_per_second;
	float falloff;
	float life_span;
	float start_size;
	float end_size;
	float speed;
	uicore::Vec3f acceleration;
	std::string particle_texture;
	std::string gradient_texture;

	std::shared_ptr<ParticleEmitterPassData> pass_data;
};

class SceneParticleEmitterVisitor
{
public:
	virtual void emitter(const uicore::GraphicContextPtr &gc, const uicore::Mat4f &world_to_eye, const uicore::Mat4f &eye_to_projection, SceneParticleEmitter_Impl *emitter) = 0;
};
