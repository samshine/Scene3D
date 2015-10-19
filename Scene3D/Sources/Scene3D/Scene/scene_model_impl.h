
#pragma once

#include <memory>

class SceneImpl;
class Model;

class SceneModelImpl : public SceneModel
{
public:
	SceneModelImpl(SceneImpl *scene, std::shared_ptr<Model> model) : scene(scene), model(model) { }

	SceneImpl *scene = nullptr;
	std::shared_ptr<Model> model;
};
