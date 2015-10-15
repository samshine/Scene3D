
#pragma once

#include <string>
#include <memory>

class Scene;
class ModelData;
typedef std::shared_ptr<Scene> ScenePtr;

class SceneModel
{
public:
	static std::shared_ptr<SceneModel> create(const ScenePtr &scene, const std::string &model_name);
	static std::shared_ptr<SceneModel> create(const ScenePtr &scene, std::shared_ptr<ModelData> model_data);

	virtual ~SceneModel() { }

protected:
	SceneModel() { }
};

typedef std::shared_ptr<SceneModel> SceneModelPtr;
