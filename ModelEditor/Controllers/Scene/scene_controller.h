
#pragma once

class SceneView;
class EditorScene;

class SceneController : public clan::ViewController
{
public:
	SceneController();

private:
	std::shared_ptr<SceneView> scene_view();

	void model_data_updated();
	void map_model_updated();
	void update_scene(clan::Scene &scene, clan::GraphicContext &gc, clan::InputContext &ic);

	std::shared_ptr<EditorScene> editor_scene;
	clan::SlotContainer slots;
};
