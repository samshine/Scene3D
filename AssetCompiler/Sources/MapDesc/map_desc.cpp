
#include "precomp.h"
#include "AssetCompiler/MapDescription/map_desc.h"
#include "JsonValue/json_value.h"

using namespace uicore;

MapDesc::MapDesc()
{
}

MapDesc MapDesc::load(const std::string &filename)
{
	MapDesc desc;

	JsonValue json = JsonValue::from_json(File::read_text(filename));
	if ((std::string)json["type"] != "map")
		throw Exception("Not a map description file");
	if ((int)json["version"] != 1)
		throw Exception("Unsupported map description file version");

	desc.fbx_filename = PathHelp::make_absolute(PathHelp::get_fullpath(filename), json["fbx_filename"]);

	for (const auto &json_light : json["lights"].get_items())
	{
		MapDescLight light;
		light.mesh_light = json_light["mesh_light"].to_string();
		light.bake = json_light["bake"].to_boolean();
		desc.lights.push_back(light);
	}

	for (const auto &json_probe : json["light_probes"].get_items())
	{
		MapDescLightProbe probe;
		probe.position.x = json_probe["position"]["x"].to_float();
		probe.position.y = json_probe["position"]["y"].to_float();
		probe.position.z = json_probe["position"]["z"].to_float();
		desc.light_probes.push_back(probe);
	}

	for (const auto &json_material : json["materials"].get_items())
	{
		MapDescMaterial material;
		material.transparent = json_material["transparent"].to_boolean();
		material.two_sided = json_material["two_sided"].to_boolean();
		material.alpha_test = json_material["alpha_test"].to_boolean();
		material.mesh_material = json_material["mesh_material"].to_string();
		desc.materials.push_back(material);
	}

	for (const auto &json_object : json["objects"].get_items())
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
		object.mesh = json_object["mesh"].to_string();
		object.animation = json_object["animation"].to_string();
		object.fields = json_object["fields"];
		desc.objects.push_back(object);
	}

	for (const auto &json_emitter : json["emitters"].get_items())
	{
		MapDescParticleEmitter emitter;
		emitter.position = Vec3f(json_emitter["position"]["x"], json_emitter["position"]["y"], json_emitter["position"]["z"]);
		emitter.bone_selector = json_emitter["bone_selector"];
		emitter.size = json_emitter["size"];
		emitter.speed = json_emitter["speed"];
		emitter.spread = json_emitter["spread"];
		emitter.gravity = json_emitter["gravity"];
		emitter.longevity = json_emitter["longevity"];
		emitter.delay = json_emitter["delay"];
		emitter.color = Vec4f(json_emitter["color"]["red"], json_emitter["color"]["green"], json_emitter["color"]["blue"], json_emitter["color"]["alpha"]);
		emitter.texture = json_emitter["texture"];
		desc.emitters.push_back(emitter);
	}

	for (const auto &json_node : json["path_nodes"].get_items())
	{
		MapDescPathNode node;
		node.position.x = json_node["position"]["x"].to_float();
		node.position.y = json_node["position"]["y"].to_float();
		node.position.z = json_node["position"]["z"].to_float();
		for (const auto &json_item : json_node["connections"].get_items())
			node.connections.push_back(json_item.to_int());
		desc.path_nodes.push_back(node);
	}

	for (const auto &json_trigger : json["triggers"].get_items())
	{
		MapDescTrigger trigger;
		trigger.id = json_trigger["id"].to_string();
		trigger.direction_normal.x = json_trigger["direction_normal"]["x"].to_float();
		trigger.direction_normal.y = json_trigger["direction_normal"]["y"].to_float();
		trigger.direction_normal.z = json_trigger["direction_normal"]["z"].to_float();
		for (const auto &json_item : json_trigger["points"].get_items())
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
		json_light["mesh_light"] = light.mesh_light;
		json_light["bake"] = light.bake;
		json_lights.get_items().push_back(json_light);
	}

	JsonValue json_probes = JsonValue::array();
	for (const auto &probe : light_probes)
	{
		JsonValue json_probe = JsonValue::object();
		json_probe["position"] = JsonValue::object();
		json_probe["position"]["x"] = probe.position.x;
		json_probe["position"]["y"] = probe.position.y;
		json_probe["position"]["z"] = probe.position.z;
		json_probes.get_items().push_back(json_probe);
	}

	JsonValue json_materials = JsonValue::array();
	for (const auto &material : materials)
	{
		JsonValue json_material = JsonValue::object();
		json_material["mesh_material"] = material.mesh_material;
		json_material["transparent"] = material.transparent;
		json_material["two_sided"] = material.two_sided;
		json_material["alpha_test"] = material.alpha_test;
		json_materials.get_items().push_back(json_material);
	}

	JsonValue json_objects = JsonValue::array();
	for (const auto &object : objects)
	{
		JsonValue json_object = JsonValue::object();
		json_object["id"] = object.id;
		json_object["type"] = object.type;
		json_object["position"] = JsonValue::object();
		json_object["position"]["x"] = object.position.x;
		json_object["position"]["y"] = object.position.y;
		json_object["position"]["z"] = object.position.z;
		json_object["dir"] = object.dir;
		json_object["up"] = object.up;
		json_object["tilt"] = object.tilt;
		json_object["scale"] = object.scale;
		json_object["mesh"] = object.mesh;
		json_object["animation"] = object.animation;
		json_object["fields"] = object.fields;
		json_objects.get_items().push_back(json_object);
	}

	JsonValue json_emitters = JsonValue::array();

	JsonValue json_nodes = JsonValue::array();
	for (const auto &node : path_nodes)
	{
		JsonValue json_node = JsonValue::object();
		json_node["position"] = JsonValue::object();
		json_node["position"]["x"] = node.position.x;
		json_node["position"]["y"] = node.position.y;
		json_node["position"]["z"] = node.position.z;
		JsonValue json_connections = JsonValue::array();
		for (const auto &item : node.connections)
			json_connections.get_items().push_back(item);
		json_node["connections"] = json_connections;
		json_nodes.get_items().push_back(json_node);
	}

	JsonValue json_triggers = JsonValue::array();
	for (const auto &trigger : triggers)
	{
		JsonValue json_trigger = JsonValue::object();
		json_trigger["direction_normal"] = JsonValue::object();
		json_trigger["direction_normal"]["x"] = trigger.direction_normal.x;
		json_trigger["direction_normal"]["y"] = trigger.direction_normal.y;
		json_trigger["direction_normal"]["z"] = trigger.direction_normal.z;
		JsonValue json_points = JsonValue::array();
		for (const auto &item : trigger.points)
		{
			JsonValue json_item = JsonValue::object();
			json_item["x"] = item.x;
			json_item["y"] = item.y;
			json_item["z"] = item.z;
			json_points.get_items().push_back(json_item);
		}
		json_trigger["points"] = json_points;
		json_triggers.get_items().push_back(json_trigger);
	}

	JsonValue json = JsonValue::object();
	json["type"] = "map";
	json["version"] = 1;
	json["lights"] = json_lights;
	json["light_probes"] = json_probes;
	json["materials"] = json_materials;
	json["objects"] = json_objects;
	json["emitters"] = json_emitters;
	json["path_nodes"] = json_nodes;
	json["triggers"] = json_triggers;
	json["fbx_filename"] = PathHelp::make_relative(PathHelp::get_fullpath(filename), fbx_filename);

	File::write_text(filename, json.to_json());
}
