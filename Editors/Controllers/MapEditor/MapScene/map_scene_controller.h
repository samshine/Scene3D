
#pragma once

#include "Controllers/Workspace/workspace_controller.h"

class SceneView;

class MapSceneObject : public Physics3DDataObject
{
public:
	SceneObjectPtr scene_object;
	Physics3DObjectPtr collision_object;
};

class MapSceneController : public WorkspaceDockableController
{
public:
	MapSceneController();

private:
	std::shared_ptr<SceneView> view = std::make_shared<SceneView>();

	void map_updated();
	void update_scene(const ScenePtr &scene, const uicore::GraphicContextPtr &gc, const uicore::DisplayWindowPtr &window, const uicore::Vec2i &);

	uicore::SlotContainer slots;

	Physics3DWorldPtr collision = Physics3DWorld::create();

	std::vector<std::shared_ptr<MapSceneObject>> objects;

	std::map<std::string, SceneModelPtr> models;
	std::map<std::string, Physics3DShapePtr> shapes;
};
