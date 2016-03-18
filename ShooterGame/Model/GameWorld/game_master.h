
#pragma once

#include "player_list.h"
#include "team_list.h"

class ClientPlayerPawn;
class ServerPlayerPawn;
class RobotPlayerPawn;
class SpawnPoint;

class GameMaster : public GameObject
{
public:
	static void create(GameWorld *world);
	static GameMaster *instance(GameWorld *world);
	static GameMaster *instance(GameObject *obj);

	GameMaster(GameWorld *world);
	~GameMaster();

	void setup_game();

	void tick() override;

	std::shared_ptr<PlayerList> player_list = std::make_shared<PlayerList>();
	std::shared_ptr<TeamList> team_list = std::make_shared<TeamList>();

	std::shared_ptr<ClientPlayerPawn> client_player;
	std::map<std::string, std::shared_ptr<ServerPlayerPawn>> server_players;
	std::vector<std::shared_ptr<RobotPlayerPawn>> bots;

	std::shared_ptr<MapData> map_data;
	std::vector<std::shared_ptr<SpawnPoint>> spawn_points;
	std::vector<Physics3DObjectPtr> level_collision_objects;

	std::string announcement_text1;
	std::string announcement_text2;
	float announcement_timeout = 0.0f;
	int score = 0;

	void net_peer_connected(const std::string &peer_id);
	void net_peer_disconnected(const std::string &peer_id);

	void player_killed(std::shared_ptr<ServerPlayerPawn> player);

private:
	void on_player_killed(const std::string &sender, const uicore::JsonValue &message);
};
