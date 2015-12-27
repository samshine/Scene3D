
#include "precomp.h"
#include "game_master.h"
#include "game_world.h"
#include "server_player_pawn.h"
#include "robot_player_pawn.h"
#include "player_ragdoll.h"

using namespace uicore;

GameMaster::GameMaster(GameWorld *world) : GameObject(world)
{
}

void GameMaster::game_start()
{
	if (!world()->client)
	{
		float random = rand() / (float)RAND_MAX;
		int spawn_index = (int)std::round((world()->spawn_points.size() - 1) * random);

		auto pawn = std::make_shared<RobotPlayerPawn>(world(), "server", world()->spawn_points[spawn_index]);
		world()->add(pawn);
		server_players["server"] = pawn;
	}
}

void GameMaster::tick(const GameTick &tick)
{
	announcement_timeout = std::max(announcement_timeout - tick.time_elapsed, 0.0f);
}

void GameMaster::net_peer_connected(const std::string &peer_id)
{
	float random = rand() / (float)RAND_MAX;
	int spawn_index = (int)std::round((world()->spawn_points.size() - 1) * random);

	auto pawn = std::make_shared<ServerPlayerPawn>(world(), peer_id, world()->spawn_points[spawn_index]);
	world()->add(pawn);
	server_players[peer_id] = pawn;
	pawn->send_net_create(world()->net_tick, "all");

	for (auto it : server_players)
	{
		if (it.first != peer_id)
			it.second->send_net_create(world()->net_tick, peer_id);
	}
}

void GameMaster::net_peer_disconnected(const std::string &peer_id)
{
	auto it = server_players.find(peer_id);
	if (it != server_players.end())
	{
		world()->remove(it->second.get());
		server_players.erase(it);
	}
}

void GameMaster::net_event_received(const std::string &sender, const uicore::NetGameEvent &net_event)
{
	if (net_event.get_name() == "player-killed" && world()->client)
	{
		Vec3f position(net_event.get_argument(1), net_event.get_argument(2), net_event.get_argument(3));
		Quaternionf orientation(net_event.get_argument(7), net_event.get_argument(4), net_event.get_argument(5), net_event.get_argument(6));
		world()->add(std::make_shared<PlayerRagdoll>(world(), position + Vec3f(0.0f, 1.0f, 0.0f), orientation));

		announcement_timeout = 2.0f;
		announcement_text1 = "You killed ~qsr*Robot!";
		announcement_text2 = "Easy Kill!";
		score++;
	}
}

void GameMaster::player_killed(const GameTick &tick, std::shared_ptr<ServerPlayerPawn> server_player)
{
	if (!world()->client)
	{
		std::string peer_id = server_player->owner;

		NetGameEvent netevent("player-killed");
		netevent.add_argument(static_id);
		netevent.add_argument(server_player->get_position().x);
		netevent.add_argument(server_player->get_position().y);
		netevent.add_argument(server_player->get_position().z);
		netevent.add_argument(server_player->get_orientation().x);
		netevent.add_argument(server_player->get_orientation().y);
		netevent.add_argument(server_player->get_orientation().z);
		netevent.add_argument(server_player->get_orientation().w);

		world()->remove(server_player.get());
		world()->network->queue_event("all", netevent, tick.arrival_tick_time);

		float random = rand() / (float)RAND_MAX;
		int spawn_index = (int)std::round((world()->spawn_points.size() - 1) * random);

		if (peer_id == "server")
		{
			auto pawn = std::make_shared<RobotPlayerPawn>(world(), "server", world()->spawn_points[spawn_index]);
			world()->add(pawn);
			server_players[peer_id] = pawn;
			pawn->send_net_create(tick, "all");
		}
		else
		{
			auto pawn = std::make_shared<ServerPlayerPawn>(world(), peer_id, world()->spawn_points[spawn_index]);
			world()->add(pawn);
			server_players[peer_id] = pawn;
			pawn->send_net_create(tick, "all");
		}
	}
}
