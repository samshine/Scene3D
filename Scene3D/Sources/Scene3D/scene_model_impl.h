
#pragma once

#include <memory>

class Scene_Impl;
class Model;

class SceneModel_Impl
{
public:
	Scene_Impl *scene = nullptr;
	std::shared_ptr<Model> model;
};
