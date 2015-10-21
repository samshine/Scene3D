
#include "precomp.h"
#include "flag.h"
#include "game_world.h"
#include "game_tick.h"
#include <algorithm>

using namespace uicore;

Flag::Flag(GameWorld *world, const Vec3f &pos, const Quaternionf &orientation, const std::string &model_name, float scale, const std::string &animation, const Vec3f &collision_box_size, const std::string &team)
: GameObject(world)
{
	if (world->client)
	{
		auto model = SceneModel::create(world->client->scene, model_name);
		scene_object = SceneObject::create(world->client->scene, model, pos, orientation, Vec3f(scale));
		scene_object->play_animation(animation, true);
	}
}

Flag::~Flag()
{
}

void Flag::tick(const GameTick &tick)
{
}

void Flag::frame(float time_elapsed, float interpolated_time)
{
	if (scene_object)
		scene_object->update(time_elapsed);
}
