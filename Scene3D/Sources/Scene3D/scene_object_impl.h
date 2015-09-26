
#pragma once

#include "Scene3D/scene_light.h"
#include "Scene3D/scene_cull_provider.h"
#include "Model/model_instance.h"
#include <list>

class Scene_Impl;

class SceneObject_Impl : public SceneItem
{
public:
	SceneObject_Impl(Scene_Impl *scene);
	~SceneObject_Impl();

	void create_lights(Scene &scene_base);
	void update_lights();

	uicore::Mat4f get_object_to_world() const
	{
		return uicore::Mat4f::translate(position) * orientation.to_matrix() * uicore::Mat4f::scale(scale);
	}

	uicore::AxisAlignedBoundingBox get_aabb() const;

	Scene_Impl *scene;
	SceneCullProxy *cull_proxy;
	std::list<SceneObject_Impl *>::iterator it;

	uicore::Vec3f position;
	uicore::Quaternionf orientation;
	uicore::Vec3f scale;

	bool light_probe_receiver;

	ModelInstance instance;
	std::vector<SceneLight> lights;
};
