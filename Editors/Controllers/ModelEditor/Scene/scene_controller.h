
#pragma once

class SceneView;
class EditorScene;

class SceneController : public uicore::ViewController
{
public:
	SceneController();

private:
	std::shared_ptr<SceneView> scene_view();

	void model_data_updated();
	void map_model_updated();
	void update_scene(uicore::Scene &scene, uicore::GraphicContext &gc, uicore::DisplayWindow &ic, const uicore::Vec2i &);

	uicore::SlotContainer slots;
};
