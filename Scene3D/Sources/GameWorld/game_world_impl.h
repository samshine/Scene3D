
#pragma once

#include "GameWorld/game_world.h"
#include "Network/NetGame/event.h"
#include "Network/game_network.h"
#include "Network/lock_step_time.h"
#include "Network/game_tick.h"
#include "Physics3D/physics3d_world.h"
#include <list>

class GameWorldImpl
{
public:
	GameWorldImpl(const std::string &hostname, const std::string &port, std::shared_ptr<GameWorldClient> client);
	~GameWorldImpl();

	void add_object(GameObjectPtr obj);

	void send_event(const std::string &target, int id, const uicore::JsonValue &message);

	void tick();

	void net_event_received(const std::string &sender, const uicore::NetGameEvent &net_event);

	std::map<std::string, std::function<GameObjectPtr(const uicore::JsonValue &args)>> create_object_factory;

	std::shared_ptr<GameNetwork> network;
	std::shared_ptr<GameWorldClient> client;

	std::shared_ptr<LockStepTime> lock_step_time;
	GameTick net_tick;

	std::list<std::shared_ptr<GameObject>> objects;

	std::map<int, GameObject*> net_objects;
	int next_id = 1;

	Physics3DWorldPtr _kinematic_collision = Physics3DWorld::create();
	Physics3DWorldPtr _dynamic_collision = Physics3DWorld::create();

	uicore::SlotContainer slots;

	friend class GameObject;
};
