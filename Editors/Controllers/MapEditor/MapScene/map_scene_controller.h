
#pragma once

class SceneView;

class MapSceneController : public uicore::ViewController
{
public:
	MapSceneController();

private:
	std::shared_ptr<SceneView> scene_view();

	void map_model_data_updated();
	void update_scene(uicore::Scene &scene, uicore::GraphicContext &gc, uicore::DisplayWindow &window, const uicore::Vec2i &);

	uicore::SlotContainer slots;

	uicore::SceneObject map_object;
};
