
#include "precomp.h"
#include "spawn_point.h"

SpawnPoint::SpawnPoint(GameWorld *world, const uicore::Vec3f &pos, float dir, float up, float tilt, const std::string &team)
	: GameObject(world), pos(pos), dir(dir), up(up), tilt(tilt), team(team)
{
}

void SpawnPoint::tick()
{
}
