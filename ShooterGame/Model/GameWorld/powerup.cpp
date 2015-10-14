
#include "precomp.h"
#include "powerup.h"
#include "game_world.h"
#include "game_tick.h"
#include "Model/game.h"
#include <algorithm>

using namespace uicore;

Powerup::Powerup(GameWorld *world, const Vec3f &pos, const Quaternionf &orientation, const std::string &model_name, float scale, const std::string &animation, const Vec3f &collision_box_size, float respawn_time, const std::string &powerup_type)
: GameObject(world)
{
	if (!world->is_server)
	{
		auto model = SceneModel::create(world->game()->scene, model_name);
		scene_object = SceneObject(world->game()->scene, model, pos, orientation, Vec3f(scale));
		scene_object.play_animation(animation, true);
	}
}

Powerup::~Powerup()
{
}

void Powerup::tick(const GameTick &tick)
{
}

void Powerup::frame(float time_elapsed, float interpolated_time)
{
	if (!scene_object.is_null())
		scene_object.update(time_elapsed);
}
