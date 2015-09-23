
#pragma once

#include <list>
#include "NetGame/event.h"
#include "NetGame/client.h"
#include "NetGame/server.h"

class NetObject
{
public:
	virtual ~NetObject() { }
};

class NetTickTimer
{
public:
	NetTickTimer(uicore::Physics3DWorld collision);

	std::function<void(float, int, int)> func_game_tick; // (float time_elapsed, int local_tick_time, int server_arrival_tick_time)
	std::function<void(const std::string &)> func_net_peer_connected; // (const std::string &id)
	std::function<void(const std::string &)> func_net_peer_disconnected; // (const std::string &id)
	std::function<void(const std::string &, const uicore::NetGameEvent &)> func_net_object_create; // (const std::string &sender, const uicore::NetGameEvent &net_event)
	std::function<void(const std::string &, NetObject *, const uicore::NetGameEvent &)> func_net_object_event; // (const std::string &sender, NetObject *receiver, const uicore::NetGameEvent &net_event)

	virtual void start(std::string hostname, std::string port) = 0;
	virtual void stop() = 0;

	virtual void update() = 0;

	uicore::NetGameEvent create_event(const std::string &name);
	virtual void send_net_event(const std::string &target, const uicore::NetGameEvent &net_event) = 0;

	virtual std::string create_id() = 0;
	void add_net_object(const std::string &obj_id, NetObject *obj);
	void remove_net_object(const std::string &obj_id);

protected:
	int ticks_elapsed();
	void step_simulation(int local_tick_time, int server_arrival_tick_time);

	static const int ticks_per_second = 60;

	struct QueuedMessage
	{
		QueuedMessage(std::string sender, uicore::NetGameEvent netevent) : sender(sender), netevent(netevent) { }
		std::string sender;
		uicore::NetGameEvent netevent;
	};
	std::list<QueuedMessage> received_messages;

	unsigned int next_send_ping;

private:
	unsigned int start_time;
	unsigned int last_tick;

	uicore::Physics3DWorld collision;
	std::map<std::string, NetObject *> objects;

	int server_arrival_tick_time;
};

class NetTickTimerClient : public NetTickTimer
{
public:
	NetTickTimerClient(uicore::Physics3DWorld collision);

	void start(std::string hostname, std::string port);
	void stop();

	void update();

	void send_net_event(const std::string &target, const uicore::NetGameEvent &net_event);
	std::string create_id() { throw uicore::Exception("Client cannot create new net objects"); }

	static int actual_ping;

private:
	void on_connected();
	void on_disconnected();
	void on_event_received(const uicore::NetGameEvent &net_event);

	uicore::NetGameClient netgame;
	uicore::SlotContainer slots;

	int server_tick_time;
	int client_tick_time;
	int ping;
	int jitter;
	bool skipped_last_tick;
};

class NetTickTimerServer : public NetTickTimer
{
public:
	NetTickTimerServer(uicore::Physics3DWorld collision);

	void start(std::string hostname, std::string port);
	void stop();

	void update();

	void send_net_event(const std::string &target, const uicore::NetGameEvent &net_event);
	std::string create_id();

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
	int next_client_id;
	int next_obj_id;

	int tick_time;
};
