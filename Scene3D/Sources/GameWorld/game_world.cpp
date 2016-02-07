
#include "precomp.h"
#include "GameWorld/game_world.h"
#include "game_world_impl.h"

namespace
{
#if _MSC_VER < 1900
	std::shared_ptr<GameWorld> __declspec(thread) *instance = nullptr;
#else
	thread_local std::shared_ptr<GameWorld> *instance = nullptr;
#endif
}

std::shared_ptr<GameWorld> GameWorld::create_server(const std::string &hostname, const std::string &port)
{
	return std::make_shared<GameWorldImpl>(hostname, port, false);
}

std::shared_ptr<GameWorld> GameWorld::create_client(const std::string &hostname, const std::string &port)
{
	return std::make_shared<GameWorldImpl>(hostname, port, true);
}

std::shared_ptr<GameWorld> GameWorld::current()
{
	return instance ? *instance : nullptr;
}

void GameWorld::set_current(std::shared_ptr<GameWorld> world)
{
	if (!world)
	{
		delete instance;
		instance = nullptr;
	}
	else
	{
		if (instance)
			*instance = world;
		else
			instance = new std::shared_ptr<GameWorld>(world);
	}
}
