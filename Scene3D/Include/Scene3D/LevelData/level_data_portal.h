
#pragma once

class LevelDataPortal
{
public:
	std::vector<uicore::Vec3f> points;
	uicore::Vec4f plane;
	int front = -1;
	int back = -1;
};
