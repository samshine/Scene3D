
#pragma once

#include "game_network.h"
#include "NetGame/server.h"
#include "NetGame/connection.h"

class GameNetworkServer : public GameNetwork
{
public:
	GameNetworkServer();

	void start(std::string hostname, std::string port) override;
	void stop() override;
	void update() override;

	void queue_event(const std::string &target, const uicore::NetGameEvent &netevent, int arrive_tick_time = -1) override;
	void send_events() override;
	void receive_events(int tick_time) override;

private:
	void on_client_connected(uicore::NetGameConnection *connection);
	void on_client_disconnected(uicore::NetGameConnection *connection, const std::string &reason);
	void on_event_received(uicore::NetGameConnection *connection, const uicore::NetGameEvent &net_event);

	struct ConnectionData
	{
		ConnectionData(std::string id) : id(id), ping(0) { }
		std::string id;
		unsigned int ping;
		std::list<uicore::NetGameEvent> events;
	};

	uicore::NetGameServer netgame;
	uicore::SlotContainer slots;
	std::map<std::string, uicore::NetGameConnection *> clients;
	int next_client_id = 0;
	std::vector<std::pair<std::string, uicore::NetGameEvent> > received_messages;
};
