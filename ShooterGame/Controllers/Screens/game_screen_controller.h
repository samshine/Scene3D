
#pragma once

#include "screen_view_controller.h"
#include "Views/GameMenuView/game_menu_view.h"

class GameWorldClient;

class GameScreenController : public ScreenViewController
{
public:
	GameScreenController(std::string hostname, std::string port, bool host_game);
	~GameScreenController();

	void update() override;

private:
	void create_menus();
	void create_resources();
	void server_thread_main(std::string hostname, std::string port);
	void on_log_event(const std::string &type, const std::string &text);

	GameMenu game_menu, options_menu;
	std::shared_ptr<GameMenuView> game_menu_view = std::make_shared<GameMenuView>();

	std::shared_ptr<GameWorldClient> client;
	GameWorldPtr client_game;
	GameWorldPtr server_game;
	uicore::FontPtr font, font2, font3, font_small;
	uicore::ImagePtr crosshair;

	float update_stats_cooldown = 0.0f;
	std::vector<std::string> update_stats;
	std::vector<std::string> update_stats2;

	std::vector<FrameStatistics> frame_stats;
	std::string fps;
	std::string ping;

	std::vector<std::string> log_messages;

	std::thread server_thread;
	std::mutex server_mutex;
	bool stop_server = false;
	bool server_running = false;
	std::condition_variable server_condition_var;
	std::exception_ptr server_exception;
};
