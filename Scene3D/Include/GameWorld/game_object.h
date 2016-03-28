
#pragma once

#include <memory>
#include <string>

class GameWorld;
class GameObject;
typedef std::shared_ptr<GameObject> GameObjectPtr;

class GameObject : public std::enable_shared_from_this<GameObject>
{
public:
	GameObject(GameWorld *world);
	virtual ~GameObject();

	virtual void tick() { }
	virtual void frame() { }

	void received_event(const std::string &sender, const uicore::JsonValue &message);
	void send_event(const std::string &target, const uicore::JsonValue &message);

	GameWorld *game_world() const { return _game_world; }

	void create_net_id();
	void set_net_id(int id);

	void remove();

	float time_elapsed() const;
	int tick_time() const;
	int send_tick_time() const;

	float frame_time_elapsed() const;
	float frame_interpolated_time() const;

	std::function<void(const std::string &sender, const uicore::JsonValue &message)> &func_received_event(const std::string &name) { return _func_received_event[name]; }

	uicore::Signal<void(const std::string &peer_id)> &sig_peer_connected();
	uicore::Signal<void(const std::string &peer_id)> &sig_peer_disconnected();
	std::function<GameObjectPtr(const uicore::JsonValue &args)> &func_create_object(const std::string &type);

	int net_id() const { return _net_id; }

	template<typename T> std::shared_ptr<T> cast() { return std::dynamic_pointer_cast<T>(shared_from_this()); }
	template<typename T> std::shared_ptr<T> cast() const { return std::dynamic_pointer_cast<T>(shared_from_this()); }

protected:
	uicore::SlotContainer slots;

private:
	GameWorld *_game_world;
	int _net_id = 0;
	bool _remove_flag = false;

	std::map<std::string, std::function<void(const std::string &sender, const uicore::JsonValue &message)>> _func_received_event;

	friend class GameWorld;
	friend class GameWorldImpl;
};
