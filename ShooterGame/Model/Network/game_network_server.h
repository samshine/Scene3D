
#pragma once

#include "game_network.h"

class GameNetworkServer : public GameNetwork
{
public:
	GameNetworkServer();

	void start(std::string hostname, std::string port) override;
	void stop() override;

	void queue_event(const std::string &target, const clan::NetGameEvent &netevent, int arrive_tick_time = -1) override;
	void send_events() override;
	void receive_events(int tick_time) override;

private:
	void on_client_connected(clan::NetGameConnection *connection);
	void on_client_disconnected(clan::NetGameConnection *connection, const std::string &reason);
	void on_event_received(clan::NetGameConnection *connection, const clan::NetGameEvent &net_event);

	struct ConnectionData
	{
		ConnectionData(std::string id) : id(id), ping(0) { }
		std::string id;
		unsigned int ping;
		std::list<clan::NetGameEvent> events;
	};

	clan::NetGameServer netgame;
	clan::SlotContainer slots;
	std::map<std::string, clan::NetGameConnection *> clients;
	int next_client_id = 0;
	std::vector<std::pair<std::string, clan::NetGameEvent> > received_messages;
};
