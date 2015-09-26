
#pragma once

class PortalSector;

class Portal
{
public:
	std::vector<uicore::Vec3f> points;
	uicore::Vec4f plane;
	PortalSector *front = nullptr;
	PortalSector *back = nullptr;
};
