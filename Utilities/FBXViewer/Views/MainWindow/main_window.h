
#pragma once

#include <UICore/StandardViews/window_view.h>

class HeaderView;
class SceneView;
class DockView;

class MainWindow : public uicore::WindowView
{
public:
	MainWindow();

private:
	static uicore::WindowDescription create_desc();

	std::shared_ptr<HeaderView> header_view;
	std::shared_ptr<SceneView> scene_view;
	std::shared_ptr<DockView> dock_view;
};
