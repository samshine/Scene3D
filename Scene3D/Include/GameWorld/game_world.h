
#pragma once

#include <memory>
#include <string>

class GameObject;
typedef std::shared_ptr<GameObject> GameObjectPtr;
class Physics3DWorld;
typedef std::shared_ptr<Physics3DWorld> Physics3DWorldPtr;

class GameWorld
{
public:
	static std::shared_ptr<GameWorld> create_server(const std::string &hostname, const std::string &port);
	static std::shared_ptr<GameWorld> create_client(const std::string &hostname, const std::string &port);

	static std::shared_ptr<GameWorld> current();
	static void set_current(std::shared_ptr<GameWorld> world);

	virtual void update() = 0;

	virtual GameObjectPtr local_object(int id) = 0;
	virtual GameObjectPtr remote_object(int id) = 0;

	virtual void add_object(GameObjectPtr obj) = 0;
	virtual void add_static_object(int static_id, GameObjectPtr obj) = 0;

	virtual float time_elapsed() const = 0;
	virtual int tick_time() const = 0;
	virtual int send_tick_time() const = 0;

	virtual float frame_time_elapsed() const = 0;
	virtual float frame_interpolated_time() const = 0;

	virtual Physics3DWorldPtr kinematic_collision() const = 0;
	virtual Physics3DWorldPtr dynamic_collision() const = 0;

	virtual uicore::Signal<void(const std::string &peer_id)> &sig_peer_connected() = 0;
	virtual uicore::Signal<void(const std::string &peer_id)> &sig_peer_disconnected() = 0;
	virtual std::function<GameObjectPtr(const uicore::JsonValue &args)> &func_create_object(const std::string &type) = 0;
};

typedef std::shared_ptr<GameWorld> GameWorldPtr;
