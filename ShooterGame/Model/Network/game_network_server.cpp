
#include "precomp.h"
#include "game_network_server.h"

using namespace clan;

GameNetworkServer::GameNetworkServer()
{
	slots.connect(netgame.sig_client_connected(), this, &GameNetworkServer::on_client_connected);
	slots.connect(netgame.sig_client_disconnected(), this, &GameNetworkServer::on_client_disconnected);
	slots.connect(netgame.sig_event_received(), this, &GameNetworkServer::on_event_received);
}

void GameNetworkServer::start(std::string hostname, std::string port)
{
	if (hostname.empty() || hostname == "*")
		netgame.start(port);
	else
		netgame.start(hostname, port);
}

void GameNetworkServer::stop()
{
	netgame.stop();
}

void GameNetworkServer::update()
{
	netgame.process_events();
}

void GameNetworkServer::queue_event(const std::string &target, const NetGameEvent &netevent, int arrive_tick_time)
{
	NetGameEvent ticked_netevent(netevent.get_name());
	ticked_netevent.add_argument(arrive_tick_time);
	for (unsigned int i = 0; i < netevent.get_argument_count(); i++)
		ticked_netevent.add_argument(netevent.get_argument(i));

	if (target == "all")
	{
		netgame.send_event(ticked_netevent);
	}
	else if (!target.empty() && target[0] == '!')
	{
		std::string not_target = target.substr(1);
		for (auto it = clients.begin(); it != clients.end(); ++it)
		{
			if (it->first != not_target)
				it->second->send_event(ticked_netevent);
		}
	}
	else
	{
		auto it = clients.find(target);
		if (it == clients.end())
			return; //throw Exception("Unknown target");
		it->second->send_event(ticked_netevent);
	}
}

void GameNetworkServer::send_events()
{
	// To do: add flush feature to netgame
}

void GameNetworkServer::receive_events(int tick_time)
{
	bool missed_deadline = false;

	auto it = received_messages.begin();
	while (it != received_messages.end())
	{
		if ((*it).second.get_argument(0).get_integer() == -1 || (*it).second.get_argument(0).get_integer() <= tick_time)
		{
			if ((*it).second.get_argument(0).get_integer() != -1 && (*it).second.get_argument(0).get_integer() < tick_time)
				missed_deadline = true;

			NetGameEvent netevent((*it).second.get_name());
			for (unsigned int i = 1; i < (*it).second.get_argument_count(); i++)
				netevent.add_argument((*it).second.get_argument(i));

			sig_event_received((*it).first, netevent);

			it = received_messages.erase(it);
		}
		else
		{
			++it;
		}
	}

	if (missed_deadline)
		Console::write_line("A client missed its delivery deadline in tick %1", tick_time);
}

void GameNetworkServer::on_client_connected(NetGameConnection *connection)
{
	std::string id = string_format("client%1", next_client_id++);
	clients[id] = connection;
	connection->set_data("data", new ConnectionData(id));

	sig_peer_connected(id);
}

void GameNetworkServer::on_client_disconnected(NetGameConnection *connection, const std::string &reason)
{
	ConnectionData *data = static_cast<ConnectionData*>(connection->get_data("data"));
	std::string id = data->id;
	delete data;
	clients.erase(clients.find(id));

	sig_peer_disconnected(id);
}

void GameNetworkServer::on_event_received(NetGameConnection *connection, const clan::NetGameEvent &net_event)
{
	ConnectionData *data = static_cast<ConnectionData*>(connection->get_data("data"));
	std::string sender = data->id;

	received_messages.push_back(std::pair<std::string, clan::NetGameEvent>(sender, net_event));
}
