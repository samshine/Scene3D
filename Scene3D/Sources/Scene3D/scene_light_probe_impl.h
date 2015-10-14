
#pragma once

#include "Scene3D/scene_light_probe.h"
#include "Scene3D/scene_cull_provider.h"
#include <list>

class Scene_Impl;

class SceneLightProbe_Impl : public SceneLightProbe, public SceneItem
{
public:
	SceneLightProbe_Impl(Scene_Impl *scene);
	~SceneLightProbe_Impl();

	uicore::Vec3f position() const override { return _position; }
	float radius() const override { return _radius; }
	uicore::Vec3f color() const override { return _color; }

	void set_position(const uicore::Vec3f &position) override;
	void set_radius(float radius) override;
	void set_color(const uicore::Vec3f &color) override { _color = color; }

	uicore::AxisAlignedBoundingBox get_aabb();

	Scene_Impl *scene = nullptr;
	SceneCullProxy *cull_proxy = nullptr;
	std::list<SceneLightProbe_Impl *>::iterator it;

	uicore::Vec3f _position;
	float _radius = 1.0f;
	uicore::Vec3f _color;
};
