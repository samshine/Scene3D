
#pragma once

#include "Model/ClientWorld/client_world.h"

class Flag : public GameObject, public ClientObject
{
public:
	Flag(const uicore::Vec3f &pos, const uicore::Quaternionf &orientation, const std::string &model_name, float scale, const std::string &animation, const uicore::Vec3f &collision_box_size, const std::string &team);
	~Flag();

	void tick() override;
	void frame(float time_elapsed, float interpolated_time) override;

protected:
	SceneObjectPtr scene_object;
};
