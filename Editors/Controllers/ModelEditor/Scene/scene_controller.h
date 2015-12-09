
#pragma once

#include "Controllers/Workspace/workspace_controller.h"

class SceneView;
class SceneRotateAction;
class EditorScene;

class SceneController : public WorkspaceDockableController
{
public:
	SceneController();

private:
	void setup_scene();
	void model_data_updated();
	void map_model_updated();
	void update_scene(const SceneViewportPtr &scene_viewport, const uicore::GraphicContextPtr &gc, const uicore::DisplayWindowPtr &window);

	std::shared_ptr<SceneView> view = std::make_shared<SceneView>();
	std::shared_ptr<SceneRotateAction> rotate_action = std::make_shared<SceneRotateAction>();

	ScenePtr scene;
	SceneLightPtr light1;
	SceneLightPtr light2;
	SceneModelPtr model1;
	SceneObjectPtr object1;
	std::vector<SceneObjectPtr> attachments;
	std::string current_animation = "default";

	float mouse_speed_x = 0.2f;
	float mouse_speed_y = 0.2f;
	float move_speed = 5.0f;

	uicore::Vec3f position;
	uicore::Vec3f rotation;

	uicore::GameTime gametime;

	uicore::SlotContainer slots;
};
