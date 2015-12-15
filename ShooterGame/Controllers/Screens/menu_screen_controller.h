
#pragma once

#include "screen_view_controller.h"

class MenuScreenController : public ScreenViewController
{
public:
	MenuScreenController();

	void update() override;

private:
	ScenePtr scene;
	SceneCameraPtr scene_camera;
	SceneObjectPtr map_object;

	uicore::FontPtr font;

	float t = 0.0f;
};
