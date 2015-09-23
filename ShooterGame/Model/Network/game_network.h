
#pragma once

#include "NetGame/event.h"

class GameNetwork
{
public:
	virtual ~GameNetwork() { }

	uicore::Signal<void(const std::string &)> sig_peer_connected; // (const std::string &id)
	uicore::Signal<void(const std::string &)> sig_peer_disconnected; // (const std::string &id)
	uicore::Signal<void(const std::string &, const uicore::NetGameEvent &)> sig_event_received; // (const std::string &sender, const uicore::NetGameEvent &net_event)

	virtual void start(std::string hostname, std::string port) = 0;
	virtual void stop() = 0;
	virtual void update() = 0;

	virtual void queue_event(const std::string &target, const uicore::NetGameEvent &netevent, int arrive_tick_time = -1) = 0;
	virtual void send_events() = 0;
	virtual void receive_events(int tick_time) = 0;
};
