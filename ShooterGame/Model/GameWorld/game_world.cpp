
#include "precomp.h"
#include "game_world.h"
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
#include "robot_player_pawn.h"
#include "Model/Network/game_network_client.h"
#include "Model/Network/game_network_server.h"
#include "Model/Network/lock_step_client_time.h"
#include "Model/Network/lock_step_server_time.h"
#include <unordered_map>

using namespace uicore;

GameWorld::GameWorld(const std::string &hostname, const std::string &port, const std::shared_ptr<GameWorldClient> &client) : client(client)
{
	if (!client)
		network = std::make_shared<GameNetworkServer>();
	else
		network = std::make_shared<GameNetworkClient>();

	slots.connect(network->sig_peer_connected, this, &GameWorld::net_peer_connected);
	slots.connect(network->sig_peer_disconnected, this, &GameWorld::net_peer_disconnected);
	slots.connect(network->sig_event_received, this, &GameWorld::net_event_received);

	if (!client)
		lock_step_time = std::make_shared<LockStepServerTime>(network);
	else
		lock_step_time = std::make_shared<LockStepClientTime>(network);

	network->start(hostname, port);

	weapon_data = JsonValue::parse(File::read_all_text("Resources/Config/weapon_data.json"));
	game_data = JsonValue::parse(File::read_all_text("Resources/Config/game.json"));

	std::string map_name = game_data["map"].to_string();

	map_data = MapData::load(PathHelp::combine("Resources/Assets", map_name + ".cmap"));

	std::unordered_map<std::string, Physics3DShapePtr> level_shapes;

	for (const auto &item : map_data->objects)
	{
		if (item.type != "Static")
			continue;

		Vec3f position = item.position;
		Vec3f scale = Vec3f(item.scale);
		Vec3f rotate(item.dir, item.up, item.tilt);
		std::string model_name = item.mesh;

		auto it = level_shapes.find(model_name);
		if (it == level_shapes.end())
		{
			std::shared_ptr<ModelData> model_data = ModelData::load(PathHelp::combine("Resources/Assets", model_name));

			level_shapes[model_name] = Physics3DShape::model(model_data);
			it = level_shapes.find(model_name);
		}

		level_collision_objects.push_back(Physics3DObject::rigid_body(collision, Physics3DShape::scale_model(it->second, scale), 0.0f, position, Quaternionf(rotate.y, rotate.x, rotate.z, angle_degrees, order_YXZ)));
	}

	for (auto obj : level_collision_objects)
	{
		obj->set_static_object();
	}

	if (client)
	{
		if (game_data["music"].to_boolean())
		{
			client->music_player.play(
			{
				"Resources/Assets/Music/game1.ogg",
				"Resources/Assets/Music/game2.ogg",
				"Resources/Assets/Music/game3.ogg"
			}, true);
		}

		std::vector<Colorf> colors;
		colors.push_back(Colorf(0.001f, 0.002f, 0.02f, 1.0f));
		colors.push_back(Colorf(0.001f, 0.002f, 0.01f, 1.0f));
		colors.push_back(Colorf(0.001f, 0.002f, 0.005f, 1.0f));
		colors.push_back(Colorf(0.001f, 0.002f, 0.01f, 1.0f));
		colors.push_back(Colorf(0.001f, 0.002f, 0.02f, 1.0f));
		client->scene->set_skybox_gradient(colors);

		for (const auto &item : map_data->objects)
		{
			if (item.type != "Static" || item.fields["no_render"].to_boolean())
				continue;

			Vec3f position = item.position;
			Vec3f scale = Vec3f(item.scale);
			Vec3f rotate(item.dir, item.up, item.tilt);
			std::string model_name = item.mesh;
			std::string animation_name = item.animation;
			client->objects.push_back(SceneObject::create(client->scene, SceneModel::create(client->scene, model_name), position, Quaternionf(rotate.y, rotate.x, rotate.z, angle_degrees, order_YXZ), scale));
			client->objects.back()->play_animation(animation_name, true);
		}

		auto json = JsonValue::parse(File::read_all_text("Resources/Config/input.json"));
		client->buttons.load(client->window, json["buttons"]);

		auto lights = std::make_shared<AlarmLights>(this);
		add(lights);
	}

	int level_obj_id = 0;

	for (const auto &objdesc : map_data->objects)
	{
		Quaternionf orientation(objdesc.up, objdesc.dir, objdesc.tilt, angle_degrees, order_YXZ);

		auto &fields = objdesc.fields;
		
		if (objdesc.type == "Elevator")
		{
			Vec3f pos2(fields["endPosition"]["x"].to_float(), fields["endPosition"]["y"].to_float(), fields["endPosition"]["z"].to_float());

			auto elevator = std::make_shared<Elevator>(this, level_obj_id++, objdesc.position, pos2, orientation, objdesc.mesh, objdesc.scale);
			add(elevator);

			elevators[elevator->level_obj_id] = elevator;
		}
		else if (objdesc.type == "Powerup")
		{
			std::string powerup_type = fields["powerupType"].to_string();
			Vec3f collision_box_size(fields["collisionBoxSize"]["x"].to_float(), fields["collisionBoxSize"]["y"].to_float(), fields["collisionBoxSize"]["z"].to_float());
			float respawn_time = fields["respawnTime"].to_float();

			auto powerup = std::make_shared<Powerup>(this, objdesc.position, orientation, objdesc.mesh, objdesc.scale, objdesc.animation, collision_box_size, respawn_time, powerup_type);
			add(powerup);
		}
		else if (objdesc.type == "Flag")
		{
			Vec3f collision_box_size(fields["collisionBoxSize"]["x"].to_float(), fields["collisionBoxSize"]["y"].to_float(), fields["collisionBoxSize"]["z"].to_float());
			std::string team = fields["team"].to_string();

			auto flag = std::make_shared<Flag>(this, objdesc.position, orientation, objdesc.mesh, objdesc.scale, objdesc.animation, collision_box_size, team);
			add(flag);
		}
		else if (objdesc.type == "SpawnPoint")
		{
			std::string team = fields["team"].to_string();

			auto spawn = std::make_shared<SpawnPoint>(this, objdesc.position, objdesc.dir, objdesc.up, objdesc.tilt, team);
			add(spawn);
			spawn_points.push_back(spawn);
		}
	}

	if (!client)
	{
		float random = rand() / (float)RAND_MAX;
		int spawn_index = (int)std::round((spawn_points.size() - 1) * random);

		auto pawn = std::make_shared<RobotPlayerPawn>(this, "server", spawn_points[spawn_index]);
		add(pawn);
		server_player_pawns["server"] = pawn;
	}
	else
	{
		float random = rand() / (float)RAND_MAX;
		int spawn_index = (int)std::round((spawn_points.size() - 1) * random);
		add(std::make_shared<PlayerRagdoll>(this, spawn_points[spawn_index]->pos + Vec3f(0.0f, 1.0f, 0.0f), Quaternionf()));
	}
}

GameWorld::~GameWorld()
{
}

void GameWorld::update(uicore::Vec2i new_mouse_movement)
{
	ScopeTimeFunction();

	mouse_movement = new_mouse_movement;
	if (client)
		client->buttons.update(client->window);

	network->update();

	lock_step_time->update();
	elapsed_timer.update();

	float time_elapsed = elapsed_timer.get_time_elapsed();

	int ticks = lock_step_time->get_ticks_elapsed();
	for (int i = 0; i < ticks; i++)
	{
		int receive_tick_time = lock_step_time->get_receive_tick_time() + i;
		int arrival_tick_time = lock_step_time->get_arrival_tick_time() + i;
		float tick_time_elapsed = lock_step_time->get_tick_time_elapsed();

		collision->step_simulation_once(tick_time_elapsed);
		net_tick = GameTick(tick_time_elapsed, receive_tick_time, arrival_tick_time);
		network->receive_events(receive_tick_time);
		tick(tick_time_elapsed, receive_tick_time, arrival_tick_time);
	}

	if (client)
	{
		for (auto &object : client->objects)
			object->update(time_elapsed);

		frame(time_elapsed, lock_step_time->get_tick_interpolation_time());

		client->music_player.update();

		client->audio->set_listener(client->scene_camera->position(), client->scene_camera->orientation());
		client->audio->update();
	}
}

void GameWorld::add(std::shared_ptr<GameObject> obj)
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

std::shared_ptr<GameObject> GameWorld::get(int id)
{
	auto it = objects.find(id);
	if (it != objects.end() && it->second->_id != 0)
		return it->second;

	it = added_objects.find(id);
	if (it != objects.end() && it->second->_id != 0)
		return it->second;

	return nullptr;
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
		if (it != objects.end())
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

	auto pawn = std::make_shared<ServerPlayerPawn>(this, peer_id, spawn_points[spawn_index]);
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
		remove(it->second.get());
		server_player_pawns.erase(it);
	}
}

void GameWorld::net_event_received(const std::string &sender, const uicore::NetGameEvent &net_event)
{
	if (!client)
	{
		if (net_event.get_name() == "player-pawn-input")
		{
			auto pawn = server_player_pawns[sender];
			if (pawn)
				pawn->net_input(net_tick, net_event);
		}
	}
	else
	{
		if (net_event.get_name() == "player-pawn-create")
		{
			int server_obj_id = net_event.get_argument(0);

			auto pawn = std::make_shared<ClientPlayerPawn>(this);
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
				auto pawn = it->second;
				client_player_pawns.erase(it);
				remove(pawn.get());
			}
		}
		else if (net_event.get_name() == "elevator-update")
		{
			int level_obj_id = net_event.get_argument(0);
			elevators[level_obj_id]->net_update(net_tick, net_event);
		}
	}
}

void GameWorld::player_killed(const GameTick &tick, std::shared_ptr<PlayerPawn> player)
{
	if (!client)
	{
		auto server_player = std::dynamic_pointer_cast<ServerPlayerPawn>(player);
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
		remove(server_player.get());


		float random = rand() / (float)RAND_MAX;
		int spawn_index = (int)std::round((spawn_points.size() - 1) * random);

		auto pawn = std::make_shared<ServerPlayerPawn>(this, peer_id, spawn_points[spawn_index]);
		add(pawn);
		server_player_pawns[peer_id] = pawn;
		pawn->send_net_create(tick, "all");
	}
}
