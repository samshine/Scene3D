
#include "precomp.h"
#include "powerup.h"
#include <algorithm>

using namespace uicore;

Powerup::Powerup(const Vec3f &pos, const Quaternionf &orientation, const std::string &model_name, float scale, const std::string &animation, const Vec3f &collision_box_size, float respawn_time, const std::string &powerup_type)
{
	if (client_world())
	{
		auto model = SceneModel::create(client_world()->scene, model_name);
		scene_object = SceneObject::create(client_world()->scene, model, pos, orientation, Vec3f(scale));
		scene_object->play_animation(animation, true);
	}
}

Powerup::~Powerup()
{
}

void Powerup::tick()
{
}

void Powerup::frame(float time_elapsed, float interpolated_time)
{
	if (scene_object)
		scene_object->update(time_elapsed);
}
