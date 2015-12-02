
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
	std::shared_ptr<SceneView> view = std::make_shared<SceneView>();
	std::shared_ptr<SceneRotateAction> rotate_action = std::make_shared<SceneRotateAction>();

	void setup_scene();
	void model_data_updated();
	void map_model_updated();
	void update_scene(const SceneViewportPtr &scene_viewport, const uicore::GraphicContextPtr &gc, const uicore::DisplayWindowPtr &ic);

	ScenePtr scene;
	uicore::Vec2i mouse_delta;
	uicore::SlotContainer slots;
};
