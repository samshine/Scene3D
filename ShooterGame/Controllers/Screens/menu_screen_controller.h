
#pragma once

#include "screen_view_controller.h"
#include "Model/Input/input_buttons.h"
#include "Model/Audio/music_player.h"
#include <stack>

class GameMenuItem
{
public:
	GameMenuItem(std::string name, std::function<void()> click = std::function<void()>(), std::function<std::string()> value = std::function<std::string()>()) : name(name), click(click), value(value) { }

	std::string name;
	std::function<void()> click;
	std::function<std::string()> value;
};

typedef std::vector<GameMenuItem> GameMenu;

class MenuScreenController : public ScreenViewController
{
public:
	MenuScreenController();

	void update() override;

private:
	void create_menus();
	void push_menu(GameMenu *menu);
	void pop_menu();
	void begin_edit(std::string *value);

	void on_key_down(const uicore::InputEvent &e);
	void on_mouse_up(const uicore::InputEvent &e);
	void on_mouse_move(const uicore::InputEvent &e);

	ScenePtr scene;
	SceneCameraPtr scene_camera;
	std::vector<SceneObjectPtr> scene_objects;

	uicore::FontPtr font, font_h1;

	float fade_time = -2.0f;
	float t = 0.0f;

	MusicPlayer music_player;

	GameMenu main_menu, join_menu, host_menu, options_menu, graphics_menu, audio_menu, controls_menu;
	std::stack<GameMenu *> menu_stack;

	int current_menu_index = 0;
	bool edit_mode = false;
	std::string *edit_value = nullptr;
	float blink = 0.0f;
	std::vector<uicore::Rectf> item_boxes;

	std::string player_name = "Player1";
	std::string server = "localhost";
	std::string port = "5004";
};
