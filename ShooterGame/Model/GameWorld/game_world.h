
#pragma once

#include <string>
#include <map>
#include "player_list.h"
#include "team_list.h"
#include "game_tick.h"
#include "Model/Network/NetGame/event.h"

class Game;
class GameTick;
class GameObject;
class PlayerPawn;
class ServerPlayerPawn;
class ClientPlayerPawn;
class Elevator;
class SpawnPoint;

class GameWorld
{
public:
	GameWorld(Game *game);
	~GameWorld();

	void init(bool is_server);

	Game *game() { return _game; }
	GameObject *get(int id);

	JsonValue weapon_data;

	std::unique_ptr<PlayerList> player_list;
	std::unique_ptr<TeamList> team_list;

	std::map<int, Elevator *> elevators;

	std::map<int, ClientPlayerPawn *> client_player_pawns;
	std::map<std::string, ServerPlayerPawn *> server_player_pawns;
	std::vector<SpawnPoint *> spawn_points;

	uicore::Point mouse_movement;

	bool is_server = false;

	GameTick net_tick;

	void tick(float time_elapsed, int time_step, int server_arrive_time_step);
	void frame(float time_elapsed, float interpolated_time);

	void net_peer_connected(const std::string &peer_id);
	void net_peer_disconnected(const std::string &peer_id);
	void net_event_received(const std::string &sender, const uicore::NetGameEvent &net_event);

	void add(GameObject *obj);
	void remove(GameObject *obj);

	void player_killed(const GameTick &tick, PlayerPawn *player);

private:
	Game *_game;
	std::map<int, GameObject *> objects;
	std::map<int, GameObject *> added_objects;
	std::vector<int> delete_list;
	int next_id;

	int stupid_counter = 0;

	friend class GameObject;
};
