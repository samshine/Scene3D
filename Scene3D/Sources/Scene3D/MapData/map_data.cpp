
#include "precomp.h"
#include "Scene3D/MapData/map_data.h"

using namespace uicore;

class CMapFormat
{
public:
	static void save(const IODevicePtr &device, std::shared_ptr<MapData> data);
	static std::shared_ptr<MapData> load(const std::string &filename);
	static std::shared_ptr<MapData> load(const IODevicePtr &device);

private:
	static std::string read_string_a(const IODevicePtr &file);
	static void write_string_a(const IODevicePtr &file, const std::string &text);
};

/////////////////////////////////////////////////////////////////////////

void MapData::save(const IODevicePtr &device, std::shared_ptr<MapData> data)
{
	CMapFormat::save(device, data);
}

std::shared_ptr<MapData> MapData::load(const std::string &filename)
{
	return CMapFormat::load(filename);
}

std::shared_ptr<MapData> MapData::load(const IODevicePtr &device)
{
	return CMapFormat::load(device);
}

/////////////////////////////////////////////////////////////////////////

inline void CMapFormat::save(const IODevicePtr &file, std::shared_ptr<MapData> data)
{
	file->write_uint32(1); // version number
	file->write("MapCaramba", 10); // file magic

	file->write_uint32(data->objects.size());
	for (const auto &obj : data->objects)
	{
		write_string_a(file, obj.id);
		write_string_a(file, obj.type);
		file->write_float(obj.position.x);
		file->write_float(obj.position.y);
		file->write_float(obj.position.z);
		file->write_float(obj.dir);
		file->write_float(obj.up);
		file->write_float(obj.tilt);
		file->write_float(obj.scale);
		write_string_a(file, obj.mesh);
		write_string_a(file, obj.animation);
		write_string_a(file, obj.fields.to_json());
	}

	file->write_uint32(data->path_nodes.size());
	for (const auto &path : data->path_nodes)
	{
		file->write_float(path.position.x);
		file->write_float(path.position.y);
		file->write_float(path.position.z);
		file->write_uint32(path.connections.size());
		for (const auto &connection : path.connections)
			file->write_int32(connection);
	}
}

inline std::shared_ptr<MapData> CMapFormat::load(const std::string &filename)
{
	return load(File::open_existing(filename));
}

inline std::shared_ptr<MapData> CMapFormat::load(const IODevicePtr &file)
{
	int version = file->read_uint32();
	char magic[10];
	file->read(magic, 10);
	if (memcmp(magic, "MapCaramba", 10) != 0)
		throw Exception("Not a Caramba Map file");

	if (version < 1 || version > 1)
		throw Exception("Unsupported file version");

	std::shared_ptr<MapData> data(new MapData());

	uint32_t num_objects = file->read_uint32();
	for (uint32_t i = 0; i < num_objects; i++)
	{
		MapDataObject obj;
		obj.id = read_string_a(file);
		obj.type = read_string_a(file);
		obj.position.x = file->read_float();
		obj.position.y = file->read_float();
		obj.position.z = file->read_float();
		obj.dir = file->read_float();
		obj.up = file->read_float();
		obj.tilt = file->read_float();
		obj.scale = file->read_float();
		obj.mesh = read_string_a(file);
		obj.animation = read_string_a(file);
		obj.fields = JsonValue::parse(read_string_a(file));
		data->objects.push_back(obj);
	}

	uint32_t num_path_nodes = file->read_uint32();
	for (uint32_t i = 0; i < num_path_nodes; i++)
	{
		MapDataPathNode path;
		path.position.x = file->read_float();
		path.position.y = file->read_float();
		path.position.z = file->read_float();

		uint32_t num_connections = file->read_uint32();
		for (uint32_t j = 0; j < num_connections; j++)
			path.connections.push_back(file->read_int32());

		data->path_nodes.push_back(path);
	}

	return data;
}

std::string CMapFormat::read_string_a(const IODevicePtr &file)
{
	std::string s;
	s.resize(file->read_uint32());
	if (!s.empty())
		file->read(&s[0], s.size());
	return s;
}

void CMapFormat::write_string_a(const IODevicePtr &file, const std::string &text)
{
	file->write_uint32(text.length());
	file->write(text.data(), text.length());
}
