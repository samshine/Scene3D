
#pragma once

class HeaderView;
class WorkspaceController;
class MapSceneController;
class MapEditController;

class MapEditorWindow : public uicore::WindowController
{
public:
	MapEditorWindow();

private:
	void update_window_title();
	void on_open();
	void on_save();
	void on_save_as();
	void on_show_edit();

	std::shared_ptr<uicore::ColumnView> view = std::make_shared<uicore::ColumnView>();
	std::shared_ptr<HeaderView> header_view;
	std::shared_ptr<WorkspaceController> workspace_controller;

	std::shared_ptr<MapSceneController> scene_controller;
	std::shared_ptr<MapEditController> edit_controller;

	uicore::SlotContainer slots;
};
