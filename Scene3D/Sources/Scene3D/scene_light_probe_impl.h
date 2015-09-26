
#pragma once

#include "Scene3D/scene_cull_provider.h"
#include <list>

class Scene_Impl;

class SceneLightProbe_Impl : public SceneItem
{
public:
	SceneLightProbe_Impl(Scene_Impl *scene);
	~SceneLightProbe_Impl();

	uicore::AxisAlignedBoundingBox get_aabb();

	Scene_Impl *scene;
	SceneCullProxy *cull_proxy;
	std::list<SceneLightProbe_Impl *>::iterator it;

	uicore::Vec3f position;
	float radius;
	uicore::Vec3f color;
};
