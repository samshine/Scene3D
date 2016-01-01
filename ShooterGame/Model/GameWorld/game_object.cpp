
#include "precomp.h"
#include "game_object.h"
#include "game_world.h"

GameObject::GameObject(GameWorld *world) : _world(world)
{
}

GameObject::~GameObject()
{
}

const GameTick &GameObject::game_tick() const
{
	return world()->net_tick;
}

float GameObject::time_elapsed() const
{
	return game_tick().time_elapsed;
}

int GameObject::receive_tick_time() const
{
	return game_tick().receive_tick_time;
}

int GameObject::arrival_tick_time() const
{
	return game_tick().arrival_tick_time;
}
