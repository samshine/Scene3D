
#pragma once

#include <string>
#include <memory>

class Scene;
class ModelData;
class SceneModel_Impl;

class SceneModel
{
public:
	SceneModel();
	SceneModel(Scene &scene, const std::string &model_name);
	SceneModel(Scene &scene, std::shared_ptr<ModelData> model_data);

	bool is_null() const;

private:
	std::shared_ptr<SceneModel_Impl> impl;

	friend class SceneObject;
};
