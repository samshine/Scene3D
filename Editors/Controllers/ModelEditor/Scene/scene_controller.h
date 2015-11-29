
#pragma once

#include "Controllers/Workspace/workspace_controller.h"

class SceneView;
class EditorScene;

class SceneController : public WorkspaceDockableController
{
public:
	SceneController();

private:
	std::shared_ptr<SceneView> view = std::make_shared<SceneView>();

	void setup_scene();
	void model_data_updated();
	void map_model_updated();
	void update_scene(const SceneViewportPtr &scene_viewport, const uicore::GraphicContextPtr &gc, const uicore::DisplayWindowPtr &ic, const uicore::Vec2i &);

	ScenePtr scene;
	uicore::SlotContainer slots;
};
