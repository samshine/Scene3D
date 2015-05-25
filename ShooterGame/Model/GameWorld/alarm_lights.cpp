
#include "precomp.h"
#include "alarm_lights.h"
#include "game_world.h"
#include "Model/game.h"

using namespace clan;

AlarmLights::AlarmLights(GameWorld *world) : GameObject(world)
{
	for (auto group : world->game()->level_data["alarmLights"].get_members())
	{
		std::vector<SceneObject> lights;

		for (auto member : group.second.get_items())
		{
			SceneModel model(world->game()->gc, world->game()->scene, member["mesh"].to_string());
			SceneObject light(world->game()->scene, model);
			light.set_scale(Vec3f(member["scale"].to_float()));
			light.set_position(Vec3f(member["position"]["x"].to_float(), member["position"]["y"].to_float(), member["position"]["z"].to_float()));
			light.set_orientation(Quaternionf(member["orientation"]["up"].to_float(), member["orientation"]["dir"].to_float(), member["orientation"]["tilt"].to_float(), angle_degrees, order_YXZ));
			light.play_animation("blinking", true);

			lights.push_back(light);
		}

		groups[group.first] = lights;
	}
}

AlarmLights::~AlarmLights()
{
}

void AlarmLights::tick(const GameTick &tick)
{
}

void AlarmLights::frame(float time_elapsed, float interpolated_time)
{
	for (auto it : groups)
	{
		for (SceneObject &obj : it.second)
		{
			obj.update(time_elapsed);
		}
	}
}

void AlarmLights::play_group_animation(const std::string &group_id, const std::string &animation_name)
{
	auto it = groups.find(group_id);
	if (it != groups.end())
	{
		for (SceneObject &obj : it->second)
		{
			obj.play_animation(animation_name, true);
		}
	}
}
