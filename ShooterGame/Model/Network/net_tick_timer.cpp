
#include "precomp.h"
#include "net_tick_timer.h"

using namespace clan;

NetTickTimer::NetTickTimer(clan::Physics3DWorld collision)
: start_time(0), last_tick(0), collision(collision), server_arrival_tick_time(0), next_send_ping(0)
{
	start_time = (unsigned int)System::get_time();
}

int NetTickTimer::ticks_elapsed()
{
	unsigned int current_time = (unsigned int)System::get_time();
	if (current_time < start_time)
	{
		start_time = current_time;
		last_tick = 0;
	}

	unsigned int cur_tick = (current_time - start_time) * ticks_per_second / 1000;
	unsigned int ticks_elapsed = cur_tick - last_tick;
	last_tick = cur_tick;

	return ticks_elapsed;
}

void NetTickTimer::step_simulation(int local_tick_time, int new_server_arrival_tick_time)
{
	float time_elapsed = 1.0f / ticks_per_second;
	collision.step_simulation_once(time_elapsed);

	server_arrival_tick_time = new_server_arrival_tick_time;

	std::list<QueuedMessage>::iterator it = received_messages.begin();
	while (it != received_messages.end())
	{
		QueuedMessage &message = *it;
		int message_tick_time = message.netevent.get_argument(0);
		if (message_tick_time > local_tick_time)
			break;

		// To do: should we drop messages from the past?

		if (message.netevent.get_name() == "ObjectEvent")
		{
			std::string obj_id = message.netevent.get_argument(1);
			std::map<std::string, NetObject *>::iterator it = objects.find(obj_id);
			if (it != objects.end())
			{
				func_net_object_event(message.sender, it->second, message.netevent);
			}
		}
		else if (message.netevent.get_name() == "ObjectCreate")
		{
			std::string obj_id = message.netevent.get_argument(1);
			std::map<std::string, NetObject *>::iterator it = objects.find(obj_id);
			if (it == objects.end())
				func_net_object_create(message.sender, message.netevent);
		}

		it = received_messages.erase(it);
	}

	func_game_tick(time_elapsed, local_tick_time, server_arrival_tick_time);
}

NetGameEvent NetTickTimer::create_event(const std::string &name)
{
	NetGameEvent e(name);
	e.add_argument(server_arrival_tick_time);
	return e;
}

void NetTickTimer::add_net_object(const std::string &obj_id, NetObject *obj) 
{
	if (objects.find(obj_id) != objects.end())
		throw Exception("NetObject ID already in use!");
	objects[obj_id] = obj;
}

void NetTickTimer::remove_net_object(const std::string &obj_id)
{
	objects.erase(objects.find(obj_id));
}

/////////////////////////////////////////////////////////////////////////////

NetTickTimerClient::NetTickTimerClient(Physics3DWorld collision)
: NetTickTimer(collision), server_tick_time(0), client_tick_time(0), ping(0), jitter(2), skipped_last_tick(false)
{
	slots.connect(netgame.sig_connected(), this, &NetTickTimerClient::on_connected);
	slots.connect(netgame.sig_disconnected(), this, &NetTickTimerClient::on_disconnected);
	slots.connect(netgame.sig_event_received(), this, &NetTickTimerClient::on_event_received);
}

void NetTickTimerClient::start(std::string hostname, std::string port)
{
	netgame.connect(hostname, port);
}

void NetTickTimerClient::stop()
{
	netgame.disconnect();
}

void NetTickTimerClient::update()
{
	// Snap local time to server time if too much out of sync
	if (std::abs(server_tick_time - client_tick_time) > 10)
	{
		client_tick_time = server_tick_time - jitter * 2;
		Console::write_line("Client snapped to server time");
	}

	// Fetch how many ticks elapsed since last
	int ticks = ticks_elapsed();

	// Run simulation for the ticks required
	for (int i = 0; i < ticks; i++)
	{
		int ticks_to_simulate = 1;
/*		if (client_tick_time <= server_tick_time - jitter * 3)
		{
			// We are slightly behind of where we should be. Tick faster.
			ticks_to_simulate = 2;
		}
		else if (client_tick_time >= server_tick_time - jitter * 1)
		{
			// We are slightly ahead of where we should be. Tick slower.
			ticks_to_simulate = skipped_last_tick ? 1 : 0;
			skipped_last_tick = !skipped_last_tick;
		}*/

		for (int j = 0; j < ticks_to_simulate; j++)
		{
			client_tick_time++;
			server_tick_time++;

			// Calculate what time sent messages should arrive on the server
			int send_tick_time = client_tick_time + ping * ticks_per_second / 1000 + jitter;

			step_simulation(client_tick_time, send_tick_time);
		}

		if (next_send_ping == 0)
		{
			netgame.send_event(NetGameEvent("Ping", { (unsigned int)System::get_time() }));
			next_send_ping = 30;
		}
		next_send_ping--;
	}
}

void NetTickTimerClient::send_net_event(const std::string &target, const NetGameEvent &net_event)
{
	if (target == "all" || target == "server")
	{
		netgame.send_event(net_event);
	}
	else
	{
		throw Exception("Unknown target");
	}
}

void NetTickTimerClient::on_connected()
{
	func_net_peer_connected("server");
}

void NetTickTimerClient::on_disconnected()
{
	func_net_peer_disconnected("server");
}

void NetTickTimerClient::on_event_received(const NetGameEvent &net_event)
{
	if (net_event.get_name() == "Ping")
	{
		server_tick_time = net_event.get_argument(1);

		NetGameEvent pong("Pong");
		for (unsigned int i = 0; i < net_event.get_argument_count(); i++)
			pong.add_argument(net_event.get_argument(i));
		netgame.send_event(pong);
	}
	else if (net_event.get_name() == "Pong")
	{
		unsigned int send_time = net_event.get_argument(0);
		ping = (unsigned int)(System::get_time() - send_time);
		actual_ping = ping;

		ping = 150;
	}
	else
	{
		// To do: sort while inserting (not needed until we use UDP instead of TCP)
		received_messages.push_back(QueuedMessage("server", net_event));
	}
}

int NetTickTimerClient::actual_ping = 0;

/////////////////////////////////////////////////////////////////////////////

NetTickTimerServer::NetTickTimerServer(Physics3DWorld collision)
: NetTickTimer(collision), next_client_id(0), next_obj_id(0), tick_time(0)
{
	slots.connect(netgame.sig_client_connected(), this, &NetTickTimerServer::on_client_connected);
	slots.connect(netgame.sig_client_disconnected(), this, &NetTickTimerServer::on_client_disconnected);
	slots.connect(netgame.sig_event_received(), this, &NetTickTimerServer::on_event_received);
}

void NetTickTimerServer::start(std::string hostname, std::string port)
{
	if (hostname.empty() || hostname == "*")
		netgame.start(port);
	else
		netgame.start(hostname, port);
}

void NetTickTimerServer::stop()
{
	netgame.stop();
}

void NetTickTimerServer::update()
{
	// Fetch how many ticks elapsed since last
	int ticks = ticks_elapsed();

	if (ticks > 1)
		Console::write_line("Update needed to do %1 ticks", ticks);

	// Run simulation for the ticks required
	for (int i = 0; i < ticks; i++)
	{
		// To do: send time sync message

		tick_time++;
		step_simulation(tick_time, tick_time);

		if (next_send_ping == 0)
		{
			netgame.send_event(NetGameEvent("Ping", { (unsigned int)System::get_time(), tick_time }));
			next_send_ping = 30;
		}
		next_send_ping--;
	}
}

void NetTickTimerServer::send_net_event(const std::string &target, const NetGameEvent &net_event)
{
	if (target == "all")
	{
		netgame.send_event(net_event);
	}
	else if (!target.empty() && target[0] == '!')
	{
		std::string not_target = target.substr(1);
		for (auto it = clients.begin(); it != clients.end(); ++it)
		{
			if (it->first != not_target)
				it->second->send_event(net_event);
		}
	}
	else
	{
		auto it = clients.find(target);
		if (it == clients.end())
			throw Exception("Unknown target");
		it->second->send_event(net_event);
	}
}

void NetTickTimerServer::on_client_connected(NetGameConnection *connection)
{
	std::string id = string_format("client%1", next_client_id++);
	clients[id] = connection;
	connection->set_data("data", new ConnectionData(id));

	func_net_peer_connected(id);
}

void NetTickTimerServer::on_client_disconnected(NetGameConnection *connection, const std::string &reason)
{
	ConnectionData *data = static_cast<ConnectionData*>(connection->get_data("data"));
	std::string id = data->id;
	delete data;
	clients.erase(clients.find(id));

	func_net_peer_disconnected(id);
}

void NetTickTimerServer::on_event_received(NetGameConnection *connection, const NetGameEvent &net_event)
{
	ConnectionData *data = static_cast<ConnectionData*>(connection->get_data("data"));

	if (net_event.get_name() == "Ping")
	{
		NetGameEvent pong("Pong");
		for (unsigned int i = 0; i < net_event.get_argument_count(); i++)
			pong.add_argument(net_event.get_argument(i));
		connection->send_event(pong);
	}
	else if (net_event.get_name() == "Pong")
	{
		unsigned int send_time = net_event.get_argument(0);
		data->ping = (unsigned int)(System::get_time() - send_time);
	}
	else
	{
		// To do: sort while inserting (not needed until we use UDP instead of TCP)
		received_messages.push_back(QueuedMessage(data->id, net_event));
	}
}

std::string NetTickTimerServer::create_id()
{
	return string_format("obj%1", next_obj_id++);
}
