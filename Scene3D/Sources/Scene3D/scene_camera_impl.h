
#pragma once

#include "Scene3D/scene_camera.h"

class Scene_Impl;

class SceneCamera_Impl : public SceneCamera
{
public:
	SceneCamera_Impl(Scene_Impl *scene) : scene(scene) { }

	uicore::Vec3f position() const override { return _position; }
	uicore::Quaternionf orientation() const override { return _orientation; }

	float field_of_view() const override { return _fov; }

	void set_position(const uicore::Vec3f &position) override { _position = position; }
	void set_orientation(const uicore::Quaternionf &orientation) override { _orientation = orientation; }

	void set_field_of_view(float fov) override { _fov = fov; }

	Scene_Impl *scene = nullptr;

	uicore::Vec3f _position;
	uicore::Quaternionf _orientation;
	float _fov = 60.0f;
};
