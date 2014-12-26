
#include "precomp.h"
#include "AssetCompiler/FBXModel/fbx_model_desc.h"

namespace clan
{
	FBXModelDesc::FBXModelDesc()
	{
		animations.push_back(FBXAnimation());
		animations[0].name = "default";
	}

	FBXModelDesc FBXModelDesc::load(const std::string &filename)
	{
		FBXModelDesc desc;
		desc.animations.clear();

		JsonValue json = JsonValue::from_json(File::read_text(filename));
		if ((std::string)json["type"] != "fbx-model")
			throw Exception("Not a model description file");
		if ((int)json["version"] != 1)
			throw Exception("Unsupported model description file version");

		desc.fbx_filename = PathHelp::make_absolute(PathHelp::get_fullpath(filename), json["fbx_filename"]);

		for (const auto &json_animation : json["animations"].get_items())
		{
			FBXAnimation animation;
			animation.name = json_animation["name"];
			animation.start_frame = json_animation["start_frame"];
			animation.end_frame = json_animation["end_frame"];
			animation.play_speed = json_animation["play_speed"];
			animation.move_speed = json_animation["move_speed"];
			animation.loop = json_animation["loop"].to_boolean();
			animation.rarity = json_animation["rarity"];
			desc.animations.push_back(animation);
		}

		for (const auto &json_attachment : json["attachment_points"].get_items())
		{
			FBXAttachmentPoint attachment;
			attachment.bone_name = json_attachment["bone_name"];
			attachment.name = json_attachment["name"];
			attachment.orientation = Quaternionf(json_attachment["orientation"]["w"], json_attachment["orientation"]["x"], json_attachment["orientation"]["y"], json_attachment["orientation"]["z"]);
			attachment.position = Vec3f(json_attachment["position"]["x"], json_attachment["position"]["y"], json_attachment["position"]["z"]);
			desc.attachment_points.push_back(attachment);
		}

		if (json.get_members().find("materials") != json.get_members().end())
		{
			for (const auto &json_material : json["materials"].get_items())
			{
				FBXMaterial material;
				if (json_material.get_members().find("transparent") != json_material.get_members().end())
					material.transparent = json_material["transparent"].to_boolean();
				material.two_sided = json_material["two_sided"].to_boolean();
				material.alpha_test = json_material["alpha_test"].to_boolean();
				material.mesh_material = json_material["mesh_material"];
				desc.materials.push_back(material);
			}
		}

		for (const auto &json_emitter : json["emitters"].get_items())
		{
			FBXParticleEmitter emitter;
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

		return desc;
	}

	void FBXModelDesc::save(const std::string &filename)
	{
		JsonValue json_animations = JsonValue::array();
		for (const auto &animation : animations)
		{
			JsonValue json_animation = JsonValue::object();
			json_animation["name"] = animation.name;
			json_animation["start_frame"] = animation.start_frame;
			json_animation["end_frame"] = animation.end_frame;
			json_animation["play_speed"] = animation.play_speed;
			json_animation["move_speed"] = animation.move_speed;
			json_animation["loop"] = animation.loop;
			json_animation["rarity"] = animation.rarity;
			json_animations.get_items().push_back(json_animation);
		}

		JsonValue json_attachment_points = JsonValue::array();
		for (const auto &attachment : attachment_points)
		{
			JsonValue json_attachment = JsonValue::object();
			json_attachment["bone_name"] = attachment.bone_name;
			json_attachment["name"] = attachment.name;
			json_attachment["orientation"] = JsonValue::object();
			json_attachment["orientation"]["x"] = attachment.orientation.x;
			json_attachment["orientation"]["y"] = attachment.orientation.y;
			json_attachment["orientation"]["z"] = attachment.orientation.z;
			json_attachment["orientation"]["w"] = attachment.orientation.w;
			json_attachment["position"] = JsonValue::object();
			json_attachment["position"]["x"] = attachment.position.x;
			json_attachment["position"]["y"] = attachment.position.y;
			json_attachment["position"]["z"] = attachment.position.z;
			json_attachment_points.get_items().push_back(json_attachment);
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

		JsonValue json_emitters = JsonValue::array();

		JsonValue json = JsonValue::object();
		json["type"] = "fbx-model";
		json["version"] = 1;
		json["animations"] = json_animations;
		json["attachment_points"] = json_attachment_points;
		json["materials"] = json_materials;
		json["emitters"] = json_emitters;
		json["fbx_filename"] = PathHelp::make_relative(PathHelp::get_fullpath(filename), fbx_filename);

		File::write_text(filename, json.to_json());
	}
}
