
#pragma once

class HeaderView;
class SceneView;
class DockView;

class MainWindow : public clan::WindowView
{
public:
	MainWindow();

	bool exit = false;

private:
	static clan::DisplayWindowDescription create_desc();

	std::shared_ptr<HeaderView> header_view;
	std::shared_ptr<SceneView> scene_view;
	std::shared_ptr<DockView> dock_view;
};
