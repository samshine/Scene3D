
#include "precomp.h"
#include "GameWorld/game_object.h"
#include "GameWorld/game_world.h"
#include "game_world_impl.h"

using namespace uicore;

GameObject::GameObject(GameWorld *world) : _game_world(world)
{
}

GameObject::~GameObject()
{
	if (_net_id != 0)
	{
		auto &net_objects = game_world()->impl->net_objects;
		auto it = net_objects.find(_net_id);
		net_objects.erase(it);
	}
}

void GameObject::send_event(const std::string &target, const JsonValue &message)
{
	if (!game_world()->client() && _net_id == 0)
		create_net_id();

	game_world()->impl->send_event(target, net_id(), message);
}

void GameObject::received_event(const std::string &sender, const uicore::JsonValue &message)
{
	auto it = _func_received_event.find(message["name"].to_string());
	if (it != _func_received_event.end())
		it->second(sender, message);
}

void GameObject::create_net_id()
{
	_net_id = game_world()->impl->next_id++;
	game_world()->impl->net_objects[_net_id] = this;
}

void GameObject::set_net_id(int id)
{
	_net_id = id;
	game_world()->impl->net_objects[_net_id] = this;
}

void GameObject::remove()
{
	_remove_flag = true;
}

float GameObject::time_elapsed() const
{
	return game_world()->time_elapsed();
}

int GameObject::tick_time() const
{
	return game_world()->tick_time();
}

int GameObject::send_tick_time() const
{
	return game_world()->send_tick_time();
}

float GameObject::frame_time_elapsed() const
{
	return game_world()->frame_time_elapsed();
}

float GameObject::frame_interpolated_time() const
{
	return game_world()->frame_interpolated_time();
}

Signal<void(const std::string &peer_id)> &GameObject::sig_peer_connected()
{
	return game_world()->sig_peer_connected();
}

Signal<void(const std::string &peer_id)> &GameObject::sig_peer_disconnected()
{
	return game_world()->sig_peer_disconnected();
}

std::function<GameObjectPtr(const JsonValue &args)> &GameObject::func_create_object(const std::string &type)
{
	return game_world()->func_create_object(type);
}
