
#include "precomp.h"
#include "flag.h"
#include <algorithm>

using namespace uicore;

Flag::Flag(GameWorld *world, const Vec3f &pos, const Quaternionf &orientation, const std::string &model_name, float scale, const std::string &animation, const Vec3f &collision_box_size, const std::string &team)
	: GameObject(world)
{
	if (game_world()->client())
	{
		auto model = SceneModel::create(game_world()->client()->scene(), model_name);
		scene_object = SceneObject::create(game_world()->client()->scene(), model, pos, orientation, Vec3f(scale));
		scene_object->play_animation(animation, true);
	}
}

Flag::~Flag()
{
}

void Flag::tick()
{
}

void Flag::frame()
{
	if (scene_object)
		scene_object->update(frame_time_elapsed());
}
