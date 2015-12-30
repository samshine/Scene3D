
#pragma once

#include "game_object.h"
#include "player_list.h"
#include "team_list.h"

class ClientPlayerPawn;
class ServerPlayerPawn;
class RobotPlayerPawn;

class GameMaster : public GameObject
{
public:
	GameMaster(GameWorld *world);

	void game_start();

	void tick(const GameTick &tick) override;
	void net_event_received(const std::string &sender, const uicore::NetGameEvent &net_event) override;

	std::shared_ptr<PlayerList> player_list = std::make_shared<PlayerList>();
	std::shared_ptr<TeamList> team_list = std::make_shared<TeamList>();

	std::shared_ptr<ClientPlayerPawn> client_player;
	std::map<std::string, std::shared_ptr<ServerPlayerPawn>> server_players;
	std::vector<std::shared_ptr<RobotPlayerPawn>> bots;

	std::string announcement_text1;
	std::string announcement_text2;
	float announcement_timeout = 0.0f;
	int score = 0;

	void net_peer_connected(const std::string &peer_id);
	void net_peer_disconnected(const std::string &peer_id);

	void player_killed(const GameTick &tick, std::shared_ptr<ServerPlayerPawn> player);

	const int static_id = -1;
};
