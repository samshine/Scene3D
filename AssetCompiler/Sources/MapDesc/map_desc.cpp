
#include "precomp.h"
#include "AssetCompiler/MapDescription/map_desc.h"

using namespace uicore;

MapDesc::MapDesc()
{
}

std::shared_ptr<MapData> MapDesc::convert(const std::string &asset_directory)
{
	auto map = std::make_shared<MapData>();
	for (const auto &obj : objects)
	{
		MapDataObject map_obj;
		map_obj.id = obj.id;
		map_obj.type = obj.type;
		map_obj.position = obj.position;
		map_obj.dir = obj.dir;
		map_obj.up = obj.up;
		map_obj.tilt = obj.tilt;
		map_obj.scale = obj.scale;
		if (!obj.mesh.empty())
			map_obj.mesh = PathHelp::combine(PathHelp::fullpath(PathHelp::make_relative(asset_directory, obj.mesh)), PathHelp::basename(obj.mesh) + ".cmodel");
		map_obj.animation = obj.animation;
		map_obj.fields = obj.fields;
		map->objects.push_back(map_obj);
	}
	for (const auto &path : path_nodes)
	{
		MapDataPathNode map_path;
		map_path.position = path.position;
		map_path.connections = path.connections;
		map->path_nodes.push_back(map_path);
	}
	return map;
}

MapDesc MapDesc::load(const std::string &filename)
{
	MapDesc desc;

	JsonValue json = JsonValue::parse(File::read_all_text(filename));
	if (json["type"].to_string() != "map")
		throw Exception("Not a map description file");
	if (json["version"].to_number() != 1)
		throw Exception("Unsupported map description file version");

	for (const auto &json_light : json["lights"].items())
	{
		MapDescLight light;
		light.mesh_light = json_light["mesh_light"].to_string();
		light.bake = json_light["bake"].to_boolean();
		desc.lights.push_back(light);
	}

	for (const auto &json_probe : json["light_probes"].items())
	{
		MapDescLightProbe probe;
		probe.position.x = json_probe["position"]["x"].to_float();
		probe.position.y = json_probe["position"]["y"].to_float();
		probe.position.z = json_probe["position"]["z"].to_float();
		desc.light_probes.push_back(probe);
	}

	for (const auto &json_material : json["materials"].items())
	{
		MapDescMaterial material;
		material.transparent = json_material["transparent"].to_boolean();
		material.two_sided = json_material["two_sided"].to_boolean();
		material.alpha_test = json_material["alpha_test"].to_boolean();
		material.mesh_material = json_material["mesh_material"].to_string();
		desc.materials.push_back(material);
	}

	for (const auto &json_object : json["objects"].items())
	{
		MapDescObject object;
		object.id = json_object["id"].to_string();
		object.type = json_object["type"].to_string();
		object.position.x = json_object["position"]["x"].to_float();
		object.position.y = json_object["position"]["y"].to_float();
		object.position.z = json_object["position"]["z"].to_float();
		object.dir = json_object["dir"].to_float();
		object.up = json_object["up"].to_float();
		object.tilt = json_object["tilt"].to_float();
		object.scale = json_object["scale"].to_float();
		object.mesh = PathHelp::make_absolute(PathHelp::fullpath(filename), json_object["mesh"].to_string());
		object.animation = json_object["animation"].to_string();
		object.fields = json_object["fields"];
		desc.objects.push_back(object);
	}

	for (const auto &json_emitter : json["emitters"].items())
	{
		MapDescParticleEmitter emitter;
		emitter.position = Vec3f(json_emitter["position"]["x"].to_float(), json_emitter["position"]["y"].to_float(), json_emitter["position"]["z"].to_float());
		emitter.bone_selector = json_emitter["bone_selector"].to_int();
		emitter.size = json_emitter["size"].to_float();
		emitter.speed = json_emitter["speed"].to_float();
		emitter.spread = json_emitter["spread"].to_float();
		emitter.gravity = json_emitter["gravity"].to_float();
		emitter.longevity = json_emitter["longevity"].to_float();
		emitter.delay = json_emitter["delay"].to_float();
		emitter.color = Vec4f(json_emitter["color"]["red"].to_float(), json_emitter["color"]["green"].to_float(), json_emitter["color"]["blue"].to_float(), json_emitter["color"]["alpha"].to_float());
		emitter.texture = json_emitter["texture"].to_string();
		desc.emitters.push_back(emitter);
	}

	for (const auto &json_node : json["path_nodes"].items())
	{
		MapDescPathNode node;
		node.position.x = json_node["position"]["x"].to_float();
		node.position.y = json_node["position"]["y"].to_float();
		node.position.z = json_node["position"]["z"].to_float();
		for (const auto &json_item : json_node["connections"].items())
			node.connections.push_back(json_item.to_int());
		desc.path_nodes.push_back(node);
	}

	for (const auto &json_trigger : json["triggers"].items())
	{
		MapDescTrigger trigger;
		trigger.id = json_trigger["id"].to_string();
		trigger.direction_normal.x = json_trigger["direction_normal"]["x"].to_float();
		trigger.direction_normal.y = json_trigger["direction_normal"]["y"].to_float();
		trigger.direction_normal.z = json_trigger["direction_normal"]["z"].to_float();
		for (const auto &json_item : json_trigger["points"].items())
		{
			Vec3f point;
			point.x = json_item["x"].to_float();
			point.y = json_item["y"].to_float();
			point.z = json_item["z"].to_float();
			trigger.points.push_back(point);
		}
		desc.triggers.push_back(trigger);
	}

	return desc;
}

void MapDesc::save(const std::string &filename)
{
	JsonValue json_lights = JsonValue::array();
	for (const auto &light : lights)
	{
		JsonValue json_light = JsonValue::object();
		json_light["mesh_light"].set_string(light.mesh_light);
		json_light["bake"].set_boolean(light.bake);
		json_lights.items().push_back(json_light);
	}

	JsonValue json_probes = JsonValue::array();
	for (const auto &probe : light_probes)
	{
		JsonValue json_probe = JsonValue::object();
		json_probe["position"] = JsonValue::object();
		json_probe["position"]["x"].set_number(probe.position.x);
		json_probe["position"]["y"].set_number(probe.position.y);
		json_probe["position"]["z"].set_number(probe.position.z);
		json_probes.items().push_back(json_probe);
	}

	JsonValue json_materials = JsonValue::array();
	for (const auto &material : materials)
	{
		JsonValue json_material = JsonValue::object();
		json_material["mesh_material"].set_string(material.mesh_material);
		json_material["transparent"].set_boolean(material.transparent);
		json_material["two_sided"].set_boolean(material.two_sided);
		json_material["alpha_test"].set_boolean(material.alpha_test);
		json_materials.items().push_back(json_material);
	}

	JsonValue json_objects = JsonValue::array();
	for (const auto &object : objects)
	{
		JsonValue json_object = JsonValue::object();
		json_object["id"].set_string(object.id);
		json_object["type"].set_string(object.type);
		json_object["position"] = JsonValue::object();
		json_object["position"]["x"].set_number(object.position.x);
		json_object["position"]["y"].set_number(object.position.y);
		json_object["position"]["z"].set_number(object.position.z);
		json_object["dir"].set_number(object.dir);
		json_object["up"].set_number(object.up);
		json_object["tilt"].set_number(object.tilt);
		json_object["scale"].set_number(object.scale);
		json_object["mesh"].set_string(PathHelp::make_relative(PathHelp::fullpath(filename), object.mesh));
		json_object["animation"].set_string(object.animation);
		json_object["fields"] = object.fields;
		json_objects.items().push_back(json_object);
	}

	JsonValue json_emitters = JsonValue::array();

	JsonValue json_nodes = JsonValue::array();
	for (const auto &node : path_nodes)
	{
		JsonValue json_node = JsonValue::object();
		json_node["position"] = JsonValue::object();
		json_node["position"]["x"].set_number(node.position.x);
		json_node["position"]["y"].set_number(node.position.y);
		json_node["position"]["z"].set_number(node.position.z);
		JsonValue json_connections = JsonValue::array();
		for (const auto &item : node.connections)
			json_connections.items().push_back(JsonValue::number(item));
		json_node["connections"] = json_connections;
		json_nodes.items().push_back(json_node);
	}

	JsonValue json_triggers = JsonValue::array();
	for (const auto &trigger : triggers)
	{
		JsonValue json_trigger = JsonValue::object();
		json_trigger["direction_normal"] = JsonValue::object();
		json_trigger["direction_normal"]["x"].set_number(trigger.direction_normal.x);
		json_trigger["direction_normal"]["y"].set_number(trigger.direction_normal.y);
		json_trigger["direction_normal"]["z"].set_number(trigger.direction_normal.z);
		JsonValue json_points = JsonValue::array();
		for (const auto &item : trigger.points)
		{
			JsonValue json_item = JsonValue::object();
			json_item["x"].set_number(item.x);
			json_item["y"].set_number(item.y);
			json_item["z"].set_number(item.z);
			json_points.items().push_back(json_item);
		}
		json_trigger["points"] = json_points;
		json_triggers.items().push_back(json_trigger);
	}

	JsonValue json = JsonValue::object();
	json["type"].set_string("map");
	json["version"].set_number(1);
	json["lights"] = json_lights;
	json["light_probes"] = json_probes;
	json["materials"] = json_materials;
	json["objects"] = json_objects;
	json["emitters"] = json_emitters;
	json["path_nodes"] = json_nodes;
	json["triggers"] = json_triggers;

	File::write_all_text(filename, json.to_json());
}
