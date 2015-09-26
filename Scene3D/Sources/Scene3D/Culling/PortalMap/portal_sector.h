
#pragma once

class PortalMapObject;
class Portal;

class PortalSector
{
public:
	std::vector<PortalMapObject *> objects;
	std::vector<Portal *> portals;
};
