
#include "precomp.h"
#include "game_world.h"
#include "Model/game.h"
#include "game_object.h"
#include "game_tick.h"
#include "alarm_lights.h"
#include "client_player_pawn.h"
#include "server_player_pawn.h"
#include "elevator.h"
#include "powerup.h"
#include "flag.h"
#include "spawn_point.h"
#include "player_ragdoll.h"

using namespace uicore;

GameWorld::GameWorld(Game *game) : _game(game), next_id(1)
{
	weapon_data = JsonValue::parse(File::read_all_text("Resources/Config/weapon_data.json"));
	player_list.reset(new PlayerList());
	team_list.reset(new TeamList());
}

GameWorld::~GameWorld()
{
}

void GameWorld::init(bool init_is_server)
{
	is_server = init_is_server;

	if (!is_server)
	{
		AlarmLights *lights = new AlarmLights(this);
		add(lights);
	}

	int level_obj_id = 0;

	for (auto &objdesc : game()->level_data["objects"].items())
	{
		std::string type = objdesc["type"].to_string();
		Vec3f pos(objdesc["position"]["x"].to_float(), objdesc["position"]["y"].to_float(), objdesc["position"]["z"].to_float());
		float dir = objdesc["dir"].to_float();
		float up = objdesc["up"].to_float();
		float tilt = objdesc["tilt"].to_float();
		Quaternionf orientation(up, dir, tilt, angle_degrees, order_YXZ);
		std::string mesh = objdesc["mesh"].to_string();
		std::string animation = objdesc["animation"].to_string();
		float scale = objdesc["scale"].to_float();

		auto &fields = objdesc["fields"];

		if (type == "Elevator")
		{
			Vec3f pos2(fields["endPosition"]["x"].to_float(), fields["endPosition"]["y"].to_float(), fields["endPosition"]["z"].to_float());

			Elevator *elevator = new Elevator(this, level_obj_id++, pos, pos2, orientation, mesh);
			add(elevator);

			elevators[elevator->level_obj_id] = elevator;
		}
		else if (type == "Powerup")
		{
			std::string powerup_type = fields["powerupType"].to_string();
			Vec3f collision_box_size(fields["collisionBoxSize"]["x"].to_float(), fields["collisionBoxSize"]["y"].to_float(), fields["collisionBoxSize"]["z"].to_float());
			float respawn_time = fields["respawnTime"].to_float();

			Powerup *powerup = new Powerup(this, pos, orientation, mesh, scale, animation, collision_box_size, respawn_time, powerup_type);
			add(powerup);
		}
		else if (type == "Flag")
		{
			Vec3f collision_box_size(fields["collisionBoxSize"]["x"].to_float(), fields["collisionBoxSize"]["y"].to_float(), fields["collisionBoxSize"]["z"].to_float());
			std::string team = fields["team"].to_string();

			Flag *flag = new Flag(this, pos, orientation, mesh, scale, animation, collision_box_size, team);
			add(flag);
		}
		else if (type == "SpawnPoint")
		{
			std::string team = fields["team"].to_string();

			SpawnPoint *spawn = new SpawnPoint(this, pos, dir, up, tilt, team);
			add(spawn);
			spawn_points.push_back(spawn);
		}
	}

	if (is_server)
	{
		float random = rand() / (float)RAND_MAX;
		int spawn_index = (int)std::round((spawn_points.size() - 1) * random);

		ServerPlayerPawn *pawn = new ServerPlayerPawn(this, "server", spawn_points[spawn_index]);
		add(pawn);
		server_player_pawns["server"] = pawn;
	}
}

void GameWorld::add(GameObject *obj)
{
	obj->_id = next_id++;
	added_objects[obj->_id] = obj;
}

void GameWorld::remove(GameObject *obj)
{
	if (obj->_id != 0)
	{
		delete_list.push_back(obj->_id);
		obj->_id = 0;
	}
}

GameObject *GameWorld::get(int id)
{
	auto it = objects.find(id);
	if (it != objects.end() && it->second->_id != 0)
		return it->second;

	it = added_objects.find(id);
	if (it != objects.end() && it->second->_id != 0)
		return it->second;

	return 0;
}

void GameWorld::tick(float time_elapsed, int receive_tick_time, int arrival_tick_time)
{
//	stupid_counter++;
//	if (!is_server && stupid_counter == 20 * 60)
//		add(new PlayerRagdoll(this, client_player_pawns.begin()->second->get_position() + client_player_pawns.begin()->second->get_orientation().rotate_vector(Vec3f(0.0f, 0.0f, 20.0f)), client_player_pawns.begin()->second->get_orientation()));


	for (auto it : objects)
	{
		if (it.first != 0)
			it.second->tick(GameTick(time_elapsed, receive_tick_time, arrival_tick_time));
	}

	objects.insert(added_objects.begin(), added_objects.end());
	added_objects.clear();

	for (int id : delete_list)
	{
		auto it = objects.find(id);
		delete it->second;
		it->second = 0;
		objects.erase(it);
	}
	delete_list.clear();
}

void GameWorld::frame(float time_elapsed, float interpolated_time)
{
	for (auto it : objects)
	{
		if (it.first != 0)
			it.second->frame(time_elapsed, interpolated_time);
	}
}

void GameWorld::net_peer_connected(const std::string &peer_id)
{
	float random = rand() / (float)RAND_MAX;
	int spawn_index = (int)std::round((spawn_points.size() - 1) * random);

	ServerPlayerPawn *pawn = new ServerPlayerPawn(this, peer_id, spawn_points[spawn_index]);
	add(pawn);
	server_player_pawns[peer_id] = pawn;
	pawn->send_net_create(net_tick, "all");

	for (auto it : server_player_pawns)
	{
		if (it.first != peer_id)
			it.second->send_net_create(net_tick, peer_id);
	}
}

void GameWorld::net_peer_disconnected(const std::string &peer_id)
{
	auto it = server_player_pawns.find(peer_id);
	if (it != server_player_pawns.end())
	{
		it->second->send_net_destroy(net_tick);
		remove(it->second);
		server_player_pawns.erase(it);
	}
}

void GameWorld::net_event_received(const std::string &sender, const uicore::NetGameEvent &net_event)
{
	if (is_server)
	{
		if (net_event.get_name() == "player-pawn-input")
		{
			ServerPlayerPawn *pawn = server_player_pawns[sender];
			if (pawn)
				pawn->net_input(net_tick, net_event);
		}
	}
	else
	{
		if (net_event.get_name() == "player-pawn-create")
		{
			int server_obj_id = net_event.get_argument(0);

			ClientPlayerPawn *pawn = new ClientPlayerPawn(this);
			add(pawn);
			client_player_pawns[server_obj_id] = pawn;

			pawn->net_create(net_tick, net_event);
		}
		else if (net_event.get_name() == "player-pawn-update")
		{
			int server_obj_id = net_event.get_argument(0);

			auto it = client_player_pawns.find(server_obj_id);
			if (it != client_player_pawns.end())
				it->second->net_update(net_tick, net_event);
		}
		else if (net_event.get_name() == "player-pawn-hit")
		{
			int server_obj_id = net_event.get_argument(0);

			auto it = client_player_pawns.find(server_obj_id);
			if (it != client_player_pawns.end())
				it->second->net_hit(net_tick, net_event);
		}
		else if (net_event.get_name() == "player-pawn-destroy")
		{
			int server_obj_id = net_event.get_argument(0);

			auto it = client_player_pawns.find(server_obj_id);
			if (it != client_player_pawns.end())
			{
				PlayerPawn *pawn = it->second;
				client_player_pawns.erase(it);
				remove(pawn);
			}
		}
		else if (net_event.get_name() == "elevator-update")
		{
			int level_obj_id = net_event.get_argument(0);
			elevators[level_obj_id]->net_update(net_tick, net_event);
		}
	}
}

void GameWorld::player_killed(const GameTick &tick, PlayerPawn *player)
{
	if (is_server)
	{
		ServerPlayerPawn *server_player = static_cast<ServerPlayerPawn*>(player);
		std::string peer_id = server_player->owner;

		server_player->send_net_destroy(tick);

		for (auto it = server_player_pawns.begin(); it != server_player_pawns.end(); ++it)
		{
			if (it->second == server_player)
			{
				server_player_pawns.erase(it);
				break;
			}
		}
		remove(server_player);


		float random = rand() / (float)RAND_MAX;
		int spawn_index = (int)std::round((spawn_points.size() - 1) * random);

		ServerPlayerPawn *pawn = new ServerPlayerPawn(this, peer_id, spawn_points[spawn_index]);
		add(pawn);
		server_player_pawns[peer_id] = pawn;
		pawn->send_net_create(tick, "all");
	}
}
