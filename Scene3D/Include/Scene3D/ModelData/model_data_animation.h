
#pragma once

class ModelDataAnimation
{
public:
	std::string name;
	float length = 0.0f;
	bool loop = false;
	float playback_speed = 1.0f;
	float moving_speed = 0.0f; // playback speed when used as character movement
	unsigned short rarity = 65535; // 0-65535 chance that this subanimation will be chosen
};
