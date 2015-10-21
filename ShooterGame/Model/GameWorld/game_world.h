
#pragma once

#include <string>
#include <map>
#include "player_list.h"
#include "team_list.h"
#include "game_tick.h"
#include "Model/Network/NetGame/event.h"
#include "Model/Network/game_network.h"
#include "Model/Network/lock_step_time.h"
#include "Model/Audio/music_player.h"
#include "Model/Input/input_buttons.h"

class Game;
class GameTick;
class GameObject;
class PlayerPawn;
class ServerPlayerPawn;
class ClientPlayerPawn;
class Elevator;
class SpawnPoint;

class GameWorldClient
{
public:
	GameWorldClient(const uicore::DisplayWindowPtr &window, const SceneEnginePtr &scene_engine, const std::shared_ptr<SoundCache> &sound_cache) : window(window), scene_engine(scene_engine)
	{
		audio = AudioWorld::create(sound_cache);
		scene = Scene::create(scene_engine);
	}

	uicore::DisplayWindowPtr window;

	SceneEnginePtr scene_engine;
	ScenePtr scene;
	SceneObjectPtr level_instance;
	std::vector<SceneObjectPtr> objects;

	AudioWorldPtr audio;
	MusicPlayer music_player;

	InputButtons buttons;
};

class GameWorld
{
public:
	GameWorld(const std::string &hostname, const std::string &port, const std::shared_ptr<GameWorldClient> &client = nullptr);
	~GameWorld();

	void update(uicore::Vec2i mouse_movement);

	std::shared_ptr<GameObject> get(int id);

	std::shared_ptr<GameWorldClient> client;

	std::shared_ptr<GameNetwork> network;
	std::shared_ptr<LockStepTime> lock_step_time;

	Physics3DWorldPtr collision = Physics3DWorld::create();
	std::vector<Physics3DObjectPtr> level_collision_objects;

	uicore::JsonValue game_data;
	uicore::JsonValue level_data;
	uicore::JsonValue weapon_data;

	std::shared_ptr<PlayerList> player_list = std::make_shared<PlayerList>();
	std::shared_ptr<TeamList> team_list = std::make_shared<TeamList>();

	std::map<int, std::shared_ptr<Elevator>> elevators;

	std::map<int, std::shared_ptr<ClientPlayerPawn>> client_player_pawns;
	std::map<std::string, std::shared_ptr<ServerPlayerPawn>> server_player_pawns;
	std::vector<std::shared_ptr<SpawnPoint>> spawn_points;

	uicore::Point mouse_movement;

	GameTick net_tick;

	void tick(float time_elapsed, int time_step, int server_arrive_time_step);
	void frame(float time_elapsed, float interpolated_time);

	void net_peer_connected(const std::string &peer_id);
	void net_peer_disconnected(const std::string &peer_id);
	void net_event_received(const std::string &sender, const uicore::NetGameEvent &net_event);

	void add(std::shared_ptr<GameObject> obj);
	void remove(GameObject *obj);

	void player_killed(const GameTick &tick, std::shared_ptr<PlayerPawn> player);

private:
	std::map<int, std::shared_ptr<GameObject>> objects;
	std::map<int, std::shared_ptr<GameObject>> added_objects;
	std::vector<int> delete_list;
	int next_id = 1;

	uicore::GameTime elapsed_timer;
	std::string map_cmodel_filename;
	uicore::SlotContainer slots;

	friend class GameObject;
};
