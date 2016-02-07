
#include "precomp.h"
#include "flag.h"
#include <algorithm>

using namespace uicore;

Flag::Flag(const Vec3f &pos, const Quaternionf &orientation, const std::string &model_name, float scale, const std::string &animation, const Vec3f &collision_box_size, const std::string &team)
{
	if (client_world())
	{
		auto model = SceneModel::create(client_world()->scene, model_name);
		scene_object = SceneObject::create(client_world()->scene, model, pos, orientation, Vec3f(scale));
		scene_object->play_animation(animation, true);
	}
}

Flag::~Flag()
{
}

void Flag::tick()
{
}

void Flag::frame(float time_elapsed, float interpolated_time)
{
	if (scene_object)
		scene_object->update(time_elapsed);
}
