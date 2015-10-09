
#pragma once

class Scene_Impl;

class ScenePass_Impl
{
public:
	ScenePass_Impl(Scene_Impl *scene_impl, const std::string &name) : scene_impl(scene_impl), name(name) { }

	Scene_Impl *scene_impl;
	const std::string name;
	std::function<void(const uicore::GraphicContextPtr &)> cb_run;
};
