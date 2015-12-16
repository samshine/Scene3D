
#pragma once

#include "screen_view_controller.h"
#include "Model/Input/input_buttons.h"

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

	int current_menu_index = 0;
	bool up_was_pressed = false;
	bool down_was_pressed = false;
	bool mouse_was_pressed = false;
};
