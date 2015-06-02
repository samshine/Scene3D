
#pragma once

class GameNetwork
{
public:
	virtual ~GameNetwork() { }

	clan::Signal<void(const std::string &)> sig_peer_connected; // (const std::string &id)
	clan::Signal<void(const std::string &)> sig_peer_disconnected; // (const std::string &id)
	clan::Signal<void(const std::string &, const clan::NetGameEvent &)> sig_event_received; // (const std::string &sender, const clan::NetGameEvent &net_event)

	virtual void start(std::string hostname, std::string port) = 0;
	virtual void stop() = 0;
	virtual void update() = 0;

	virtual void queue_event(const std::string &target, const clan::NetGameEvent &netevent, int arrive_tick_time = -1) = 0;
	virtual void send_events() = 0;
	virtual void receive_events(int tick_time) = 0;
};
