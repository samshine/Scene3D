
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
	bool enter_was_pressed = false;

	MusicPlayer music_player;

	std::vector<std::string> main_menu = std::vector<std::string> { "New Game", "Join Game", "Host Game", "Options", "Quit" };
	std::vector<std::string> join_menu = std::vector<std::string> { "Join Game", "Player Name:", "Server:", "Port:", "Back" };
	std::vector<std::string> host_menu = std::vector<std::string> { "Create Game", "Player Name:", "Port:", "Back" };
	std::vector<std::string> options_menu = std::vector<std::string> { "Graphics", "Audio", "Controls", "Back" };
	std::vector<std::string> graphics_menu = std::vector<std::string> { "Quality:", "Gamma:", "V-Sync:", "Back" };
	std::vector<std::string> audio_menu = std::vector<std::string> { "Master Volume:", "Music Volume:", "SFX Volume:", "Back" };
	std::vector<std::string> controls_menu = std::vector<std::string> { "Back" };

	enum class Menu { main, join, host, options, graphics, audio, controls };
	Menu current_menu = Menu::main;
	bool edit_mode = false;

	std::string player_name = "Player1";
	std::string server = "localhost";
	std::string port = "5004";
};
