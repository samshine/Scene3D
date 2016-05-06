
#include "precomp.h"
#include "game_master.h"
#include "server_player_pawn.h"
#include "client_player_pawn.h"
#include "robot_player_pawn.h"
#include "player_ragdoll.h"
#include "powerup.h"
#include "elevator.h"
#include "flag.h"
#include "spawn_point.h"
#include "rocket.h"
#include <unordered_map>

using namespace uicore;

//#define NO_BOTS

void GameMaster::create(GameWorld *world)
{
	auto game_master = std::make_shared<GameMaster>(world);
	game_master->set_net_id(-1);
	world->add_object(game_master);
	game_master->setup_game();
}

GameMaster *GameMaster::instance(GameWorld *world)
{
	return (GameMaster *)world->net_object(-1).get();
}

GameMaster *GameMaster::instance(GameObject *obj)
{
	return instance(obj->game_world());
}

GameMaster::GameMaster(GameWorld *world) : GameObject(world)
{
	func_received_event("player-killed") = bind_member(this, &GameMaster::on_player_killed);
	func_create_object("player-pawn") = [&](const JsonValue &args) -> GameObjectPtr { auto pawn = std::make_shared<ClientPlayerPawn>(game_world()); pawn->net_create(args); return pawn; };
	func_create_object("rocket") = [&](const JsonValue &args) -> GameObjectPtr { return std::make_shared<Rocket>(game_world(), args); };

	slots.connect(sig_peer_connected(), this, &GameMaster::net_peer_connected);
	slots.connect(sig_peer_disconnected(), this, &GameMaster::net_peer_disconnected);
}

GameMaster::~GameMaster()
{
}

void GameMaster::setup_game()
{
	int level_obj_id = -2;

	std::string map_name = "Levels/liandri.cmap";

	map_data = MapData::load(PathHelp::combine("Resources/Assets", map_name));

	std::unordered_map<std::string, Physics3DShapePtr> level_shapes;

	for (const auto &item : map_data->objects)
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

		level_collision_objects.push_back(Physics3DObject::rigid_body(game_world()->kinematic_collision(), shape, 0.0f, position, Quaternionf::euler(radians(rotate))));
		level_collision_objects.push_back(Physics3DObject::rigid_body(game_world()->dynamic_collision(), shape, 0.0f, position, Quaternionf::euler(radians(rotate))));
	}

	for (auto obj : level_collision_objects)
	{
		obj->set_static_object();
	}

	if (game_world()->client())
	{
		game_world()->client()->music_player().play(
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
		game_world()->client()->scene()->set_skybox_gradient(colors);

		for (const auto &item : map_data->objects)
		{
			if (item.type != "Static" && item.type != "Level")
				continue;

			Vec3f position = item.position;
			Vec3f scale = Vec3f(item.scale);
			Vec3f rotate(item.up, item.dir, item.tilt);
			std::string model_name = item.mesh;
			std::string animation_name = item.animation;
			game_world()->client()->objects().push_back(SceneObject::create(game_world()->client()->scene(), SceneModel::create(game_world()->client()->scene(), model_name), position, Quaternionf::euler(rotate), scale));
			game_world()->client()->objects().back()->play_animation(animation_name, true);
		}

		game_world()->client()->load_buttons("Resources/Config/input.json");
	}

	for (const auto &objdesc : map_data->objects)
	{
		auto orientation = Quaternionf::euler(radians(objdesc.up), radians(objdesc.dir), radians(objdesc.tilt));

		auto &fields = objdesc.fields;

		if (objdesc.type == "Elevator")
		{
			Vec3f pos2(fields["endPosition"]["x"].to_float(), fields["endPosition"]["y"].to_float(), fields["endPosition"]["z"].to_float());
			auto elevator = std::make_shared<Elevator>(game_world(), objdesc.position, pos2, orientation, objdesc.mesh, objdesc.scale);
			elevator->set_net_id(level_obj_id--);
			game_world()->add_object(elevator);
		}
		else if (objdesc.type == "Powerup")
		{
			std::string powerup_type = fields["powerupType"].to_string();
			Vec3f collision_box_size(fields["collisionBoxSize"]["x"].to_float(), fields["collisionBoxSize"]["y"].to_float(), fields["collisionBoxSize"]["z"].to_float());
			float respawn_time = fields["respawnTime"].to_float();
			auto powerup = std::make_shared<Powerup>(game_world(), objdesc.position, orientation, objdesc.mesh, objdesc.scale, objdesc.animation, collision_box_size, respawn_time, powerup_type);
			powerup->set_net_id(level_obj_id--);
			game_world()->add_object(powerup);
		}
		else if (objdesc.type == "Flag")
		{
			Vec3f collision_box_size(fields["collisionBoxSize"]["x"].to_float(), fields["collisionBoxSize"]["y"].to_float(), fields["collisionBoxSize"]["z"].to_float());
			std::string team = fields["team"].to_string();
			auto flag = std::make_shared<Flag>(game_world(), objdesc.position, orientation, objdesc.mesh, objdesc.scale, objdesc.animation, collision_box_size, team);
			flag->set_net_id(level_obj_id--);
			game_world()->add_object(flag);
		}
		else if (objdesc.type == "SpawnPoint")
		{
			std::string team = fields["team"].to_string();

			auto spawn = std::make_shared<SpawnPoint>(game_world(), objdesc.position, objdesc.dir, objdesc.up, objdesc.tilt, team);
			spawn->set_net_id(level_obj_id--);
			game_world()->add_object(spawn);
			spawn_points.push_back(spawn);
		}
	}

#ifndef NO_BOTS
	if (!game_world()->client())
	{
		for (int i = 0; i < 3; i++)
		{
			float random = rand() / (float)RAND_MAX;
			int spawn_index = (int)std::round((spawn_points.size() - 1) * random);

			auto pawn = std::make_shared<RobotPlayerPawn>(game_world(), "server", spawn_points[spawn_index]);
			game_world()->add_object(pawn);
			bots.push_back(pawn);
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
	if (game_world()->client())
		return;

	float random = rand() / (float)RAND_MAX;
	int spawn_index = (int)std::round((spawn_points.size() - 1) * random);

	auto pawn = std::make_shared<ServerPlayerPawn>(game_world(), peer_id, spawn_points[spawn_index]);
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
	if (game_world()->client())
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
	if (game_world()->client())
	{
		Vec3f position(message["x"].to_float(), message["y"].to_float(), message["z"].to_float());
		Quaternionf orientation(message["rw"].to_float(), message["rx"].to_float(), message["ry"].to_float(), message["rz"].to_float());
		game_world()->add_object(std::make_shared<PlayerRagdoll>(game_world(), position + Vec3f(0.0f, 1.0f, 0.0f), orientation));
		game_world()->net_object(message["playerid"].to_int())->remove();

		announcement_timeout = 2.0f;
		announcement_text1 = "You killed ~qsr*Robot!";
		announcement_text2 = "Easy Kill!";
		score++;
	}
}

void GameMaster::player_killed(std::shared_ptr<ServerPlayerPawn> server_player)
{
	if (!game_world()->client())
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
		message["playerid"] = JsonValue::number(server_player->net_id());

		server_player->remove();
		send_event("all", message);

		float random = rand() / (float)RAND_MAX;
		int spawn_index = (int)std::round((spawn_points.size() - 1) * random);

		if (peer_id == "server")
		{
			auto it = std::find(bots.begin(), bots.end(), server_player);
			if (it != bots.end())
				bots.erase(it);

			auto pawn = std::make_shared<RobotPlayerPawn>(game_world(), "server", spawn_points[spawn_index]);
			game_world()->add_object(pawn);
			bots.push_back(pawn);
			pawn->send_net_create("all");
		}
		else
		{
			auto pawn = std::make_shared<ServerPlayerPawn>(game_world(), peer_id, spawn_points[spawn_index]);
			game_world()->add_object(pawn);
			server_players[peer_id] = pawn;
			pawn->send_net_create("all");
		}
	}
}
