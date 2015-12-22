
#include "precomp.h"
#include "game_screen_controller.h"

using namespace uicore;

GameScreenController::GameScreenController(std::string hostname, std::string port, bool host_game)
{
	set_cursor_hidden();

	if (host_game)
		server_game = std::make_unique<GameWorld>(hostname, port);
	client_game = std::make_unique<GameWorld>(!hostname.empty() ? hostname : "localhost", port, std::make_shared<GameWorldClient>(window(), scene_engine(), sound_cache()));
}

void GameScreenController::update()
{
	server_game->update(Vec2i());
	client_game->update(mouse_delta());

	scene_viewport()->set_camera(client_game->client->scene_camera);
	scene_viewport()->update(gc(), game_time().get_time_elapsed());
	scene_viewport()->render(gc());
}
