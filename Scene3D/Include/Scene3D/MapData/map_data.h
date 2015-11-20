
#pragma once

class MapDataObject
{
public:
	std::string id;
	std::string type;
	uicore::Vec3f position;
	float dir = 0.0f;
	float up = 0.0f;
	float tilt = 0.0f;
	float scale = 1.0f;
	std::string mesh;
	std::string animation = "default";
	uicore::JsonValue fields = uicore::JsonValue::object();
};

class MapDataPathNode
{
public:
	uicore::Vec3f position;
	std::vector<int> connections;
};

/// \brief Data structure describing a map.
class MapData
{
public:
	std::vector<MapDataObject> objects;
	std::vector<MapDataPathNode> path_nodes;

	static void save(const uicore::IODevicePtr &device, std::shared_ptr<MapData> data);
	static std::shared_ptr<MapData> load(const std::string &filename);
	static std::shared_ptr<MapData> load(const uicore::IODevicePtr &device);
};
