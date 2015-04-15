
#pragma once

class SceneView;

class MapSceneController : public clan::ViewController
{
public:
	MapSceneController();

private:
	std::shared_ptr<SceneView> scene_view();

	void map_model_data_updated();
	void update_scene(clan::Scene &scene, clan::GraphicContext &gc, clan::InputContext &ic, const clan::Vec2i &);

	clan::SlotContainer slots;

	clan::SceneObject map_object;
};
