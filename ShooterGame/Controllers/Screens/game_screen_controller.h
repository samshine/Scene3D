
#pragma once

#include "screen_view_controller.h"
#include "Model/GameWorld/game_world.h"

class GameScreenController : public ScreenViewController
{
public:
	GameScreenController();
	void update() override;

private:
	std::unique_ptr<GameWorld> client_game;
	std::unique_ptr<GameWorld> server_game;
};
