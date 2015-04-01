
#pragma once

class HeaderView;
class WorkspaceController;
class MapSceneController;
class ObjectsController;

class MapEditorWindow : public clan::ViewController
{
public:
	MapEditorWindow();

	std::shared_ptr<clan::WindowView> window_view() { return std::static_pointer_cast<clan::WindowView>(view); }

private:
	void create_layout();
	void update_window_title();
	void on_open();
	void on_save();
	void on_save_as();
	void on_show_objects();
	void on_change_model();

	std::shared_ptr<HeaderView> header_view;
	std::shared_ptr<WorkspaceController> workspace_controller;

	std::shared_ptr<MapSceneController> scene_controller;
	std::shared_ptr<ObjectsController> objects_controller;

	clan::SlotContainer slots;
};