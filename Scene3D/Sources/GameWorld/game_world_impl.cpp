
#include "precomp.h"
#include "game_world_impl.h"
#include "GameWorld/game_object.h"
#include "Network/game_tick.h"
#include "Network/game_network_client.h"
#include "Network/game_network_server.h"
#include "Network/lock_step_client_time.h"
#include "Network/lock_step_server_time.h"
#include "Scene3D/Performance/scope_timer.h"

using namespace uicore;

GameWorldImpl::GameWorldImpl(const std::string &hostname, const std::string &port, bool client) : client(client)
{
	if (!client)
		network = std::make_shared<GameNetworkServer>();
	else
		network = std::make_shared<GameNetworkClient>();

	slots.connect(network->sig_event_received, this, &GameWorldImpl::net_event_received);

	if (!client)
		lock_step_time = std::make_shared<LockStepServerTime>(network);
	else
		lock_step_time = std::make_shared<LockStepClientTime>(network);

	network->start(hostname, port);
}

GameWorldImpl::~GameWorldImpl()
{
}

void GameWorldImpl::update()
{
	ScopeTimeFunction();

	network->update();

	lock_step_time->update();

	int ticks = lock_step_time->ticks_elapsed();
	for (int i = 0; i < ticks; i++)
	{
		int receive_tick_time = lock_step_time->receive_tick_time() + i;
		int arrival_tick_time = lock_step_time->arrival_tick_time() + i;
		float tick_time_elapsed = lock_step_time->tick_time_elapsed();

		net_tick = GameTick(tick_time_elapsed, receive_tick_time, arrival_tick_time);
		network->receive_events(receive_tick_time);
		tick();
	}
}

GameObjectPtr GameWorldImpl::local_object(int id)
{
	auto it = objects.find(id);
	if (it != objects.end() && it->second->local_id() != 0)
		return it->second;

	it = added_objects.find(id);
	if (it != added_objects.end() && it->second->local_id() != 0)
		return it->second;

	return nullptr;
}

GameObjectPtr GameWorldImpl::remote_object(int id)
{
	auto it = remote_objects.find(id);
	if (it != remote_objects.end() && it->second->remote_id() != 0)
		return it->second;
	return nullptr;
}

void GameWorldImpl::add_object(GameObjectPtr obj)
{
	obj->_local_id = next_id++;
	added_objects[obj->_local_id] = obj;
}

void GameWorldImpl::add_static_object(int static_id, GameObjectPtr obj)
{
	obj->_local_id = -static_id;
	added_objects[obj->_local_id] = obj;
}

void GameWorldImpl::remove_object(GameObject *obj)
{
	delete_list.push_back(obj->_local_id);
}

float GameWorldImpl::time_elapsed() const
{
	return net_tick.time_elapsed;
}

int GameWorldImpl::tick_time() const
{
	return net_tick.receive_tick_time;
}

int GameWorldImpl::send_tick_time() const
{
	return net_tick.arrival_tick_time;
}

float GameWorldImpl::frame_time_elapsed() const
{
	return lock_step_time->frame_time_elapsed();
}

float GameWorldImpl::frame_interpolated_time() const
{
	return lock_step_time->tick_interpolation_time();
}

void GameWorldImpl::tick()
{
	ScopeTimeFunction();

	for (auto it : objects)
	{
		if (it.first != 0)
			it.second->tick();
	}

	objects.insert(added_objects.begin(), added_objects.end());
	added_objects.clear();

	for (int id : delete_list)
	{
		auto it = objects.find(id);
		if (it != objects.end())
		{
			int remote_id = it->second->remote_id();
			objects.erase(it);

			if (remote_id != 0)
			{
				auto remote_it = remote_objects.find(remote_id);
				if (remote_it != remote_objects.end())
					remote_objects.erase(remote_it);
			}
		}
	}
	delete_list.clear();
}

Signal<void(const std::string &peer_id)> &GameWorldImpl::sig_peer_connected()
{
	return network->sig_peer_connected;
}

Signal<void(const std::string &peer_id)> &GameWorldImpl::sig_peer_disconnected()
{
	return network->sig_peer_disconnected;
}

void GameWorldImpl::send_event(const std::string &target, int id, const JsonValue &message)
{
	NetGameEvent net_event("GameObjectMessage", { id, message.to_json() });
	network->queue_event(target, net_event, net_tick.arrival_tick_time);
}

void GameWorldImpl::net_event_received(const std::string &sender, const NetGameEvent &net_event)
{
	try
	{
		if (net_event.get_name() != "GameObjectMessage")
			return;

		int obj_id = net_event.get_argument(0);
		JsonValue message = JsonValue::parse(net_event.get_argument(1));
		if (!message.is_object())
			return;

		if (!client)
		{
			auto it = objects.find(obj_id);
			if (it != objects.end())
				it->second->received_event(sender, message);
		}
		else
		{
			auto it = remote_objects.find(obj_id);
			if (it != remote_objects.end())
			{
				it->second->received_event(sender, message);
			}
			else if (message["type"].type() == JsonType::string && obj_id > 0)
			{
				auto it2 = create_object_factory.find(message["type"].to_string());
				if (it2 != create_object_factory.end())
				{
					auto instance = it2->second(message);
					if (instance)
					{
						add_object(instance);
						instance->_remote_id = obj_id;
						remote_objects[obj_id] = instance;
					}
				}
				/*
				std::shared_ptr<GameObject> instance;
				std::string type = net_event.get_argument(1);

				if (type == "player-pawn")
					instance = std::make_shared<ClientPlayerPawn>(this);
				else if (type == "rocket")
					instance = std::make_shared<Rocket>(this, net_event);
*/
			}
		}
	}
	catch (...)
	{
		// To do: probably should log this..
	}
}
