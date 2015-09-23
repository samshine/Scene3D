
#pragma once

#include "game_network.h"
#include "NetGame/client.h"

class GameNetworkClient : public GameNetwork
{
public:
	GameNetworkClient();

	void start(std::string hostname, std::string port) override;
	void stop() override;
	void update() override;

	void queue_event(const std::string &target, const uicore::NetGameEvent &netevent, int arrive_tick_time = -1) override;
	void send_events() override;
	void receive_events(int tick_time) override;

private:
	void on_connected();
	void on_disconnected();
	void on_event_received(const uicore::NetGameEvent &net_event);

	uicore::NetGameClient netgame;
	uicore::SlotContainer slots;

	std::vector<uicore::NetGameEvent> received_messages;
};
