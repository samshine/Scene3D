
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

	void model_data_updated();
	void map_model_updated();
	void update_scene(Scene &scene, uicore::GraphicContext &gc, uicore::DisplayWindow &ic, const uicore::Vec2i &);

	uicore::SlotContainer slots;
};
