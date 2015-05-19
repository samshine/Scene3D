
#include "precomp.h"
#include "game_network_client.h"

using namespace clan;

GameNetworkClient::GameNetworkClient()
{
	slots.connect(netgame.sig_connected(), this, &GameNetworkClient::on_connected);
	slots.connect(netgame.sig_disconnected(), this, &GameNetworkClient::on_disconnected);
	slots.connect(netgame.sig_event_received(), this, &GameNetworkClient::on_event_received);
}

void GameNetworkClient::start(std::string hostname, std::string port)
{
	netgame.connect(hostname, port);
}

void GameNetworkClient::stop()
{
	netgame.disconnect();
}

void GameNetworkClient::queue_event(const std::string &target, const NetGameEvent &netevent, int arrive_tick_time)
{
	NetGameEvent ticked_netevent(netevent.get_name());
	ticked_netevent.add_argument(arrive_tick_time);
	for (unsigned int i = 0; i < netevent.get_argument_count(); i++)
		ticked_netevent.add_argument(netevent.get_argument(i));

	netgame.send_event(ticked_netevent);
}

void GameNetworkClient::send_events()
{
	// To do: add flush feature to netgame
}

void GameNetworkClient::receive_events(int tick_time)
{
	auto it = received_messages.begin();
	while (it != received_messages.end())
	{
		if ((*it).get_argument(0).get_integer() == -1 || (*it).get_argument(0).get_integer() <= tick_time)
		{
			NetGameEvent netevent((*it).get_name());
			for (unsigned int i = 1; i < (*it).get_argument_count(); i++)
				netevent.add_argument((*it).get_argument(i));

			sig_event_received("server", netevent);

			it = received_messages.erase(it);
		}
		else
		{
			++it;
		}
	}
}

void GameNetworkClient::on_connected()
{
	sig_peer_connected("server");
}

void GameNetworkClient::on_disconnected()
{
	sig_peer_disconnected("server");
}

void GameNetworkClient::on_event_received(const NetGameEvent &net_event)
{
	received_messages.push_back(net_event);
}
