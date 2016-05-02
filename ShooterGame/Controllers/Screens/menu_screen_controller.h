
#pragma once

#include "Views/GameMenuView/game_menu_view.h"

class MenuScreenController : public ScreenController
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
	std::shared_ptr<GameMenuView> game_menu_view = std::make_shared<GameMenuView>();
};
