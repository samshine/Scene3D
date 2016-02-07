
#pragma once

#include "GameWorld/game_world.h"
#include "Network/NetGame/event.h"
#include "Network/game_network.h"
#include "Network/lock_step_time.h"
#include "Network/game_tick.h"
#include "Physics3D/physics3d_world.h"

class GameWorldImpl : public GameWorld
{
public:
	GameWorldImpl(const std::string &hostname, const std::string &port, bool client);
	~GameWorldImpl();

	void update() override;

	GameObjectPtr local_object(int id) override;
	GameObjectPtr remote_object(int id) override;

	void add_object(GameObjectPtr obj) override;
	void add_static_object(int static_id, GameObjectPtr obj) override;
	void remove_object(GameObject *);

	float time_elapsed() const override;
	int tick_time() const override;
	int send_tick_time() const override;

	float frame_time_elapsed() const override;
	float frame_interpolated_time() const override;

	Physics3DWorldPtr kinematic_collision() const override { return _kinematic_collision; }
	Physics3DWorldPtr dynamic_collision() const override { return _dynamic_collision; }

	uicore::Signal<void(const std::string &peer_id)> &sig_peer_connected() override;
	uicore::Signal<void(const std::string &peer_id)> &sig_peer_disconnected() override;
	std::function<GameObjectPtr(const uicore::JsonValue &args)> &func_create_object(const std::string &type) override { return create_object_factory[type]; }

	void send_event(const std::string &target, int id, const uicore::JsonValue &message);

private:
	void tick();

	void net_event_received(const std::string &sender, const uicore::NetGameEvent &net_event);

	std::map<std::string, std::function<GameObjectPtr(const uicore::JsonValue &args)>> create_object_factory;

	std::shared_ptr<GameNetwork> network;
	bool client = false;

	std::shared_ptr<LockStepTime> lock_step_time;
	GameTick net_tick;

	std::map<int, std::shared_ptr<GameObject>> objects;
	std::map<int, std::shared_ptr<GameObject>> added_objects;
	std::vector<int> delete_list;
	int next_id = 1;

	std::map<int, std::shared_ptr<GameObject>> remote_objects;

	Physics3DWorldPtr _kinematic_collision = Physics3DWorld::create();
	Physics3DWorldPtr _dynamic_collision = Physics3DWorld::create();

	uicore::SlotContainer slots;

	friend class GameObject;
};
