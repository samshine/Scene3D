
#include "precomp.h"
#include "game_master.h"
#include "server_player_pawn.h"
#include "robot_player_pawn.h"
#include "player_ragdoll.h"
#include "powerup.h"
#include "elevator.h"
#include "flag.h"
#include "spawn_point.h"
#include "Model/ClientWorld/client_world.h"
#include <unordered_map>

using namespace uicore;

namespace
{
#if _MSC_VER < 1900
	GameMaster __declspec(thread) *instance_ptr = nullptr;
#else
	thread_local GameMaster *instance_ptr = nullptr;
#endif
}

#define NO_BOTS

GameMaster *GameMaster::instance()
{
	return instance_ptr;
}

GameMaster::GameMaster()
{
	func_received_event("player-killed") = bind_member(this, &GameMaster::on_player_killed);
	instance_ptr = this;
}

GameMaster::~GameMaster()
{
	instance_ptr = nullptr;
}

void GameMaster::create()
{
	auto game_master = std::make_shared<GameMaster>();

	int level_obj_id = 1;
	GameWorld::current()->add_static_object(level_obj_id++, game_master);

	std::string map_name = "Levels/Liandri/liandri2.cmap";

	game_master->map_data = MapData::load(PathHelp::combine("Resources/Assets", map_name));

	std::unordered_map<std::string, Physics3DShapePtr> level_shapes;

	for (const auto &item : game_master->map_data->objects)
	{
		if (item.type != "Level")
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

		auto shape = (item.scale == 1.0f) ? it->second : Physics3DShape::scale_model(it->second, scale);

		game_master->level_collision_objects.push_back(Physics3DObject::rigid_body(game_master->game_world()->kinematic_collision(), shape, 0.0f, position, Quaternionf(rotate.y, rotate.x, rotate.z, angle_degrees, order_YXZ)));
		game_master->level_collision_objects.push_back(Physics3DObject::rigid_body(game_master->game_world()->dynamic_collision(), shape, 0.0f, position, Quaternionf(rotate.y, rotate.x, rotate.z, angle_degrees, order_YXZ)));
	}

	for (auto obj : game_master->level_collision_objects)
	{
		obj->set_static_object();
	}

	if (game_master->client_world())
	{
		game_master->client_world()->music_player.play(
		{
			"Resources/Assets/Music/game1.ogg",
			"Resources/Assets/Music/game2.ogg",
			"Resources/Assets/Music/game3.ogg"
		}, true);

		std::vector<Colorf> colors;
		colors.push_back(Colorf(0.001f, 0.002f, 0.02f, 1.0f));
		colors.push_back(Colorf(0.001f, 0.002f, 0.01f, 1.0f));
		colors.push_back(Colorf(0.001f, 0.002f, 0.005f, 1.0f));
		colors.push_back(Colorf(0.001f, 0.002f, 0.01f, 1.0f));
		colors.push_back(Colorf(0.001f, 0.002f, 0.02f, 1.0f));
		game_master->client_world()->scene->set_skybox_gradient(colors);

		for (const auto &item : game_master->map_data->objects)
		{
			if (item.type != "Static" && item.type != "Level")
				continue;

			Vec3f position = item.position;
			Vec3f scale = Vec3f(item.scale);
			Vec3f rotate(item.dir, item.up, item.tilt);
			std::string model_name = item.mesh;
			std::string animation_name = item.animation;
			game_master->client_world()->objects.push_back(SceneObject::create(game_master->client_world()->scene, SceneModel::create(game_master->client_world()->scene, model_name), position, Quaternionf(rotate.y, rotate.x, rotate.z, angle_degrees, order_YXZ), scale));
			game_master->client_world()->objects.back()->play_animation(animation_name, true);
		}

		auto json = JsonValue::parse(File::read_all_text("Resources/Config/input.json"));
		game_master->client_world()->buttons.load(game_master->client_world()->window, json["buttons"]);
	}

	for (const auto &objdesc : game_master->map_data->objects)
	{
		Quaternionf orientation(objdesc.up, objdesc.dir, objdesc.tilt, angle_degrees, order_YXZ);

		auto &fields = objdesc.fields;

		if (objdesc.type == "Elevator")
		{
			Vec3f pos2(fields["endPosition"]["x"].to_float(), fields["endPosition"]["y"].to_float(), fields["endPosition"]["z"].to_float());
			game_master->add_static_object(level_obj_id++, std::make_shared<Elevator>(objdesc.position, pos2, orientation, objdesc.mesh, objdesc.scale));
		}
		else if (objdesc.type == "Powerup")
		{
			std::string powerup_type = fields["powerupType"].to_string();
			Vec3f collision_box_size(fields["collisionBoxSize"]["x"].to_float(), fields["collisionBoxSize"]["y"].to_float(), fields["collisionBoxSize"]["z"].to_float());
			float respawn_time = fields["respawnTime"].to_float();

			game_master->add_object(std::make_shared<Powerup>(objdesc.position, orientation, objdesc.mesh, objdesc.scale, objdesc.animation, collision_box_size, respawn_time, powerup_type));
		}
		else if (objdesc.type == "Flag")
		{
			Vec3f collision_box_size(fields["collisionBoxSize"]["x"].to_float(), fields["collisionBoxSize"]["y"].to_float(), fields["collisionBoxSize"]["z"].to_float());
			std::string team = fields["team"].to_string();

			game_master->add_object(std::make_shared<Flag>(objdesc.position, orientation, objdesc.mesh, objdesc.scale, objdesc.animation, collision_box_size, team));
		}
		else if (objdesc.type == "SpawnPoint")
		{
			std::string team = fields["team"].to_string();

			auto spawn = std::make_shared<SpawnPoint>(objdesc.position, objdesc.dir, objdesc.up, objdesc.tilt, team);
			game_master->add_object(spawn);
			game_master->spawn_points.push_back(spawn);
		}
	}

#ifndef NO_BOTS
	if (!world()->client)
	{
		for (int i = 0; i < 3; i++)
		{
			float random = rand() / (float)RAND_MAX;
			int spawn_index = (int)std::round((world()->spawn_points.size() - 1) * random);

			auto pawn = std::make_shared<RobotPlayerPawn>("server", game_master->spawn_points[spawn_index]);
			game_master->add_object(pawn);
			game_master->bots.push_back(pawn);
		}
	}
#endif
}

void GameMaster::tick()
{
	announcement_timeout = std::max(announcement_timeout - time_elapsed(), 0.0f);
}

void GameMaster::net_peer_connected(const std::string &peer_id)
{
	if (client_world())
		return;

	float random = rand() / (float)RAND_MAX;
	int spawn_index = (int)std::round((spawn_points.size() - 1) * random);

	auto pawn = std::make_shared<ServerPlayerPawn>(peer_id, spawn_points[spawn_index]);
	game_world()->add_object(pawn);
	server_players[peer_id] = pawn;
	pawn->send_net_create("all");

	for (auto it : server_players)
	{
		if (it.first != peer_id)
			it.second->send_net_create(peer_id);
	}

	for (const auto &robot : bots)
	{
		robot->send_net_create(peer_id);
	}
}

void GameMaster::net_peer_disconnected(const std::string &peer_id)
{
	if (client_world())
		return;

	auto it = server_players.find(peer_id);
	if (it != server_players.end())
	{
		it->second->remove();
		server_players.erase(it);
	}
}

void GameMaster::on_player_killed(const std::string &sender, const uicore::JsonValue &message)
{
	if (client_world())
	{
		Vec3f position(message["x"].to_float(), message["y"].to_float(), message["z"].to_float());
		Quaternionf orientation(message["rw"].to_float(), message["rx"].to_float(), message["ry"].to_float(), message["rz"].to_float());
		game_world()->add_object(std::make_shared<PlayerRagdoll>(position + Vec3f(0.0f, 1.0f, 0.0f), orientation));

		announcement_timeout = 2.0f;
		announcement_text1 = "You killed ~qsr*Robot!";
		announcement_text2 = "Easy Kill!";
		score++;
	}
}

void GameMaster::player_killed(std::shared_ptr<ServerPlayerPawn> server_player)
{
	if (!client_world())
	{
		std::string peer_id = server_player->owner;

		auto message = JsonValue::object();
		message["name"] = JsonValue::string("player-killed");
		message["x"] = JsonValue::number(server_player->get_position().x);
		message["y"] = JsonValue::number(server_player->get_position().y);
		message["z"] = JsonValue::number(server_player->get_position().z);
		message["rx"] = JsonValue::number(server_player->get_orientation().x);
		message["ry"] = JsonValue::number(server_player->get_orientation().y);
		message["rz"] = JsonValue::number(server_player->get_orientation().z);
		message["rw"] = JsonValue::number(server_player->get_orientation().w);

		server_player->remove();
		send_event("all", message);

		float random = rand() / (float)RAND_MAX;
		int spawn_index = (int)std::round((spawn_points.size() - 1) * random);

		if (peer_id == "server")
		{
			auto it = std::find(bots.begin(), bots.end(), server_player);
			if (it != bots.end())
				bots.erase(it);

			auto pawn = std::make_shared<RobotPlayerPawn>("server", spawn_points[spawn_index]);
			game_world()->add_object(pawn);
			bots.push_back(pawn);
			pawn->send_net_create("all");
		}
		else
		{
			auto pawn = std::make_shared<ServerPlayerPawn>(peer_id, spawn_points[spawn_index]);
			game_world()->add_object(pawn);
			server_players[peer_id] = pawn;
			pawn->send_net_create("all");
		}
	}
}
