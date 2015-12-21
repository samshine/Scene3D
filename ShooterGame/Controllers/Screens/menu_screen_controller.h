
#pragma once

#include "screen_view_controller.h"
#include "Model/Input/input_buttons.h"
#include "Model/Audio/music_player.h"

class MenuScreenController : public ScreenViewController
{
public:
	MenuScreenController();

	void update() override;

private:
	ScenePtr scene;
	SceneCameraPtr scene_camera;
	std::vector<SceneObjectPtr> scene_objects;

	uicore::FontPtr font, font_h1;

	float fade_time = -2.0f;
	float t = 0.0f;

	int current_menu_index = 0;
	bool up_was_pressed = false;
	bool down_was_pressed = false;
	bool mouse_was_pressed = false;

	MusicPlayer music_player;
};
