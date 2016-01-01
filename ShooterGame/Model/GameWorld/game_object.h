
#pragma once

#include <string>
#include <map>
#include "Model/Network/NetGame/event.h"

class Game;
class GameWorld;
class GameTick;

class GameObject : public std::enable_shared_from_this<GameObject>
{
public:
	GameObject(GameWorld *world);
	virtual ~GameObject();

	virtual void tick() { }
	virtual void frame(float time_elapsed, float interpolated_time) { }

	virtual void net_event_received(const std::string &sender, const uicore::NetGameEvent &net_event) { }
	void send_net_event(const std::string &target, const uicore::NetGameEvent &netevent);

	const GameTick &game_tick() const;
	float time_elapsed() const;
	int receive_tick_time() const;
	int arrival_tick_time() const;

	GameWorld *world() const { return _world; }

	int id() const { return _id; }
	int remote_id() const { return _remote_id; }

	template<typename T> std::shared_ptr<T> to_type() { return std::dynamic_pointer_cast<T>(shared_from_this()); }

private:
	void set_remote_id(int id) { _remote_id = id; }

	GameWorld *_world;
	int _id = 0;
	int _remote_id = 0;

	friend class GameWorld;
};
