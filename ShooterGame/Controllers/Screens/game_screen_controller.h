
#pragma once

#include "screen_view_controller.h"
#include "Model/GameWorld/game_world.h"

class GameScreenController : public ScreenViewController
{
public:
	GameScreenController(std::string hostname, std::string port, bool host_game);
	void update() override;

private:
	void on_log_event(const std::string &type, const std::string &text);

	std::unique_ptr<GameWorld> client_game;
	std::unique_ptr<GameWorld> server_game;
	uicore::FontPtr font, font2, font3;

	std::vector<std::string> log_messages;
};
