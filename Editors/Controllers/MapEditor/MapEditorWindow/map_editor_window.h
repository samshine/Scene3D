
#pragma once

class HeaderView;
class WorkspaceController;
class MapSceneController;
class CreateController;
class ModifyController;
class ObjectsController;
class MapLightsController;
class MapLightProbesController;
class MapEmittersController;
class MapMaterialsController;
class TriggersController;
class PathNodesController;

class MapEditorWindow : public uicore::WindowController
{
public:
	MapEditorWindow();

private:
	void update_window_title();
	void on_open();
	void on_save();
	void on_save_as();
	void on_compile();
	void on_show_create();
	void on_show_modify();
	void on_show_objects();
	void on_show_lights();
	void on_show_light_probes();
	void on_show_emitters();
	void on_show_materials();
	void on_show_triggers();
	void on_show_path_nodes();

	std::shared_ptr<uicore::ColumnView> view = std::make_shared<uicore::ColumnView>();
	std::shared_ptr<HeaderView> header_view;
	std::shared_ptr<WorkspaceController> workspace_controller;

	std::shared_ptr<MapSceneController> scene_controller;
	std::shared_ptr<CreateController> create_controller;
	std::shared_ptr<ModifyController> modify_controller;
	std::shared_ptr<ObjectsController> objects_controller;
	std::shared_ptr<MapLightsController> lights_controller;
	std::shared_ptr<MapLightProbesController> light_probes_controller;
	std::shared_ptr<MapEmittersController> emitters_controller;
	std::shared_ptr<MapMaterialsController> materials_controller;
	std::shared_ptr<TriggersController> triggers_controller;
	std::shared_ptr<PathNodesController> path_nodes_controller;

	uicore::SlotContainer slots;
};
