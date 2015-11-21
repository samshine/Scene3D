
#pragma once

#include "Controllers/Workspace/workspace_controller.h"

class SceneView;

class MapSceneController : public WorkspaceDockableController
{
public:
	MapSceneController();

private:
	std::shared_ptr<SceneView> view = std::make_shared<SceneView>();

	void map_updated();
	void update_scene(const ScenePtr &scene, const uicore::GraphicContextPtr &gc, const uicore::DisplayWindowPtr &window, const uicore::Vec2i &);

	uicore::SlotContainer slots;

	std::vector<SceneObjectPtr> objects;
	std::map<std::string, SceneModelPtr> models;
};
