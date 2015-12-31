
#pragma once

#include "screen_view_controller.h"
#include "Model/GameWorld/game_world.h"

class GameScreenController : public ScreenViewController
{
public:
	GameScreenController(std::string hostname, std::string port, bool host_game, float mouse_speed_x, float mouse_speed_y);
	~GameScreenController();

	void update() override;

private:
	void server_thread_main();
	void on_log_event(const std::string &type, const std::string &text);

	std::unique_ptr<GameWorld> client_game;
	std::unique_ptr<GameWorld> server_game;
	uicore::FontPtr font, font2, font3, font_small;
	uicore::ImagePtr crosshair;

	float update_stats_cooldown = 0.0f;
	std::vector<std::string> update_stats;
	int fps_counter = 0;
	std::string fps;
	std::string ping;

	std::vector<std::string> log_messages;

	std::thread server_thread;
	std::mutex server_mutex;
	bool stop_server = false;
	std::exception_ptr server_exception;
};
