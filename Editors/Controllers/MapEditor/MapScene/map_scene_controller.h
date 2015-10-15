
#pragma once

#include "Controllers/Workspace/workspace_controller.h"

class SceneView;

class MapSceneController : public WorkspaceDockableController
{
public:
	MapSceneController();

private:
	std::shared_ptr<SceneView> view = std::make_shared<SceneView>();

	void map_model_data_updated();
	void update_scene(const ScenePtr &scene, const uicore::GraphicContextPtr &gc, const uicore::DisplayWindowPtr &window, const uicore::Vec2i &);

	uicore::SlotContainer slots;

	SceneObjectPtr map_object;
};
