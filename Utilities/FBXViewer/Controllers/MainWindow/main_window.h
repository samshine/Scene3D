
#pragma once

class HeaderView;
class WorkspaceController;
class SceneController;
class AnimationsController;
class AttachmentsController;
class MaterialsController;

class MainWindow : public clan::ViewController
{
public:
	MainWindow();

	bool exit = false;
	std::shared_ptr<clan::WindowView> window_view() { return std::static_pointer_cast<clan::WindowView>(view); }

private:
	void create_layout();
	void on_load();
	void on_save();
	void on_save_as();
	void on_show_attachments();
	void on_show_animations();
	void on_show_materials();
	void on_change_model();

	std::shared_ptr<HeaderView> header_view;
	std::shared_ptr<WorkspaceController> workspace_controller;

	std::shared_ptr<SceneController> scene_controller;
	std::shared_ptr<AnimationsController> animations_controller;
	std::shared_ptr<AttachmentsController> attachments_controller;
	std::shared_ptr<MaterialsController> materials_controller;

	clan::SlotContainer slots;
};
