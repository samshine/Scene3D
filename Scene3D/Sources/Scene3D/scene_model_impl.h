
#pragma once

#include <memory>

class Scene_Impl;
class Model;

class SceneModel_Impl : public SceneModel
{
public:
	SceneModel_Impl(Scene_Impl *scene, std::shared_ptr<Model> model) : scene(scene), model(model) { }

	Scene_Impl *scene = nullptr;
	std::shared_ptr<Model> model;
};
