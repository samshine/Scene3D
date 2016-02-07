
#pragma once

class SpawnPoint : public GameObject
{
public:
	SpawnPoint(const uicore::Vec3f &pos, float dir, float up, float tilt, const std::string &team);

	void tick() override;

	uicore::Vec3f pos;
	float dir;
	float up;
	float tilt;
	std::string team;
};
