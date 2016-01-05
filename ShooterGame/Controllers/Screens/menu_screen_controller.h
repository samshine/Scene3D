
#pragma once

#include "screen_view_controller.h"
#include "Model/Input/input_buttons.h"
#include "Model/Audio/music_player.h"
#include "Views/GameMenuView/game_menu_view.h"

class MenuScreenController : public ScreenViewController
{
public:
	MenuScreenController();

	void update() override;

private:
	void create_menus();

	ScenePtr scene;
	SceneCameraPtr scene_camera;
	std::vector<SceneObjectPtr> scene_objects;

	float fade_time = -2.0f;
	float t = 0.0f;

	MusicPlayer music_player;

	GameMenu main_menu, join_menu, host_menu, options_menu;
	GameMenuView game_menu_view;
};
