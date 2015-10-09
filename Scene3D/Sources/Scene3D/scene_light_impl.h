
#pragma once

#include "Scene3D/scene_cull_provider.h"
#include <list>

class Scene_Impl;

class SceneLight_Impl : public SceneItem
{
public:
	SceneLight_Impl(Scene_Impl *scene);
	~SceneLight_Impl();

	uicore::AxisAlignedBoundingBox get_aabb();

	Scene_Impl *scene;
	SceneCullProxy *cull_proxy;
	std::list<SceneLight_Impl *>::iterator it;

	SceneLight::Type type;
	uicore::Vec3f position;
	uicore::Quaternionf orientation;

	uicore::Vec3f color;
	float falloff;
	float hotspot;
	float ambient_illumination;
	float attenuation_start;
	float attenuation_end;
	bool rectangle_shape;
	float aspect_ratio;

	bool shadow_caster;
	bool light_caster;
	SceneLight shadow_source;

	std::unique_ptr<VSMShadowMapPassLightData> vsm_data;
};

class SceneLightVisitor
{
public:
	virtual void light(const uicore::GraphicContextPtr &gc, const uicore::Mat4f &world_to_eye, const uicore::Mat4f &eye_to_projection, SceneLight_Impl *light) = 0;
};
