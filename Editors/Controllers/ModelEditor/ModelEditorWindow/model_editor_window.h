
#pragma once

class HeaderView;
class WorkspaceController;
class SceneController;
class AnimationsController;
class AttachmentsController;
class MaterialsController;
class LightsController;
class BonesController;
class CamerasController;
class EnvironmentController;

class ModelEditorWindow : public uicore::WindowController
{
public:
	ModelEditorWindow();

private:
	void update_window_title();
	void on_open();
	void on_save();
	void on_save_as();
	void on_show_attachments();
	void on_show_animations();
	void on_show_materials();
	void on_show_lights();
	void on_show_bones();
	void on_show_cameras();
	//void on_show_environment();
	void on_change_model();

	std::shared_ptr<uicore::ColumnView> view = std::make_shared<uicore::ColumnView>();
	std::shared_ptr<HeaderView> header_view;
	std::shared_ptr<WorkspaceController> workspace_controller;

	std::shared_ptr<SceneController> scene_controller;
	std::shared_ptr<AnimationsController> animations_controller;
	std::shared_ptr<AttachmentsController> attachments_controller;
	std::shared_ptr<MaterialsController> materials_controller;
	std::shared_ptr<LightsController> lights_controller;
	std::shared_ptr<BonesController> bones_controller;
	std::shared_ptr<CamerasController> cameras_controller;
	//std::shared_ptr<EnvironmentController> environment_controller;

	uicore::SlotContainer slots;
};
