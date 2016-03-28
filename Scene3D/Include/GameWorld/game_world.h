
#pragma once

#include <memory>
#include <string>

class GameObject;
typedef std::shared_ptr<GameObject> GameObjectPtr;
class Physics3DWorld;
typedef std::shared_ptr<Physics3DWorld> Physics3DWorldPtr;
class GameWorldImpl;
class GameWorldClient;

class GameWorld
{
public:
	GameWorld(const std::string &hostname, const std::string &port, std::shared_ptr<GameWorldClient> client);
	~GameWorld();

	void update(uicore::Vec2i mouse_movement = uicore::Vec2i(), bool has_focus = false);

	const std::shared_ptr<GameWorldClient> &client();

	GameObjectPtr net_object(int id);

	void add_object(GameObjectPtr obj);

	float time_elapsed() const;
	int tick_time() const;
	int send_tick_time() const;

	float frame_time_elapsed() const;
	float frame_interpolated_time() const;

	Physics3DWorldPtr kinematic_collision() const;
	Physics3DWorldPtr dynamic_collision() const;

	uicore::Signal<void(const std::string &peer_id)> &sig_peer_connected();
	uicore::Signal<void(const std::string &peer_id)> &sig_peer_disconnected();
	std::function<GameObjectPtr(const uicore::JsonValue &args)> &func_create_object(const std::string &type);

private:
	GameWorld(const GameWorld &) = delete;
	GameWorld &operator =(const GameWorld &) = delete;

	std::unique_ptr<GameWorldImpl> impl;
	friend class GameObject;
};

typedef std::shared_ptr<GameWorld> GameWorldPtr;
