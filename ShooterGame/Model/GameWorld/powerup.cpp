
#include "precomp.h"
#include "powerup.h"
#include <algorithm>

using namespace uicore;

Powerup::Powerup(GameWorld *world, const Vec3f &pos, const Quaternionf &orientation, const std::string &model_name, float scale, const std::string &animation, const Vec3f &collision_box_size, float respawn_time, const std::string &powerup_type)
	: GameObject(world)
{
	if (game_world()->client())
	{
		auto model = SceneModel::create(game_world()->client()->scene(), model_name);
		scene_object = SceneObject::create(game_world()->client()->scene(), model, pos, orientation, Vec3f(scale));
		scene_object->play_animation(animation, true);
	}
}

Powerup::~Powerup()
{
}

void Powerup::tick()
{
}

void Powerup::frame()
{
	if (scene_object)
		scene_object->update(frame_time_elapsed());
}
