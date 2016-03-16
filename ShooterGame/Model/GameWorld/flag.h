
#pragma once

class Flag : public GameObject
{
public:
	Flag(GameWorld *world, const uicore::Vec3f &pos, const uicore::Quaternionf &orientation, const std::string &model_name, float scale, const std::string &animation, const uicore::Vec3f &collision_box_size, const std::string &team);
	~Flag();

	void tick() override;
	void frame() override;

protected:
	SceneObjectPtr scene_object;
};
