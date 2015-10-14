
#pragma once

#include <string>
#include <memory>

class Scene;
class ModelData;

class SceneModel
{
public:
	static std::shared_ptr<SceneModel> create(Scene &scene, const std::string &model_name);
	static std::shared_ptr<SceneModel> create(Scene &scene, std::shared_ptr<ModelData> model_data);

	virtual ~SceneModel() { }

protected:
	SceneModel() { }
};

typedef std::shared_ptr<SceneModel> SceneModelPtr;
