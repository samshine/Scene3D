
#pragma once

class Scene_Impl;

class SceneCamera_Impl
{
public:
	Scene_Impl *scene = nullptr;
	uicore::Vec3f position;
	uicore::Quaternionf orientation;
	float fov = 60.0f;
};
