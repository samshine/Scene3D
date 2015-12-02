
#pragma once

#include "Controllers/Workspace/workspace_controller.h"
#include "Views/Grid/grid_view.h"

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
	std::shared_ptr<GridView> grid_view = std::make_shared<GridView>();
	std::shared_ptr<SceneView> scene_views[4];

	void map_updated();
	void setup_scene();
	void update_scene(int index, const SceneViewportPtr &scene_viewport, const uicore::GraphicContextPtr &gc, const uicore::DisplayWindowPtr &window);

	uicore::SlotContainer slots;

	ScenePtr scene;

	Physics3DWorldPtr collision = Physics3DWorld::create();

	std::vector<std::shared_ptr<MapSceneObject>> objects;

	std::map<std::string, SceneModelPtr> models;
	std::map<std::string, Physics3DShapePtr> shapes;

	float mouse_speed_x = 0.2f;
	float mouse_speed_y = 0.2f;
	float move_speed = 5.0f;

	uicore::Vec3f position[4];
	uicore::Vec3f rotation[4];

	uicore::GameTime gametime;
};
