
#include "precomp.h"
#include "AssetCompiler/ModelDescription/model_desc.h"

using namespace uicore;

ModelDesc::ModelDesc()
{
	animations.push_back(ModelDescAnimation());
	animations[0].name = "default";
}

ModelDesc ModelDesc::load(const std::string &filename)
{
	ModelDesc desc;
	desc.animations.clear();

	JsonValue json = JsonValue::parse(File::read_all_text(filename));
	if (json["type"].to_string() != "fbx-model")
		throw Exception("Not a model description file");
	if (json["version"].to_number() != 1)
		throw Exception("Unsupported model description file version");

	desc.fbx_filename = PathHelp::make_absolute(PathHelp::fullpath(filename), json["fbx_filename"].to_string());

	for (const auto &json_animation : json["animations"].items())
	{
		ModelDescAnimation animation;
		animation.name = json_animation["name"].to_string();
		animation.start_frame = json_animation["start_frame"].to_int();
		animation.end_frame = json_animation["end_frame"].to_int();
		animation.play_speed = json_animation["play_speed"].to_float();
		animation.move_speed = json_animation["move_speed"].to_float();
		animation.loop = json_animation["loop"].to_boolean();
		animation.rarity = json_animation["rarity"].to_int();
		if (!json_animation["fbx_filename"].is_undefined())
		{
			animation.fbx_filename = json_animation["fbx_filename"].to_string();
			if (!animation.fbx_filename.empty())
				animation.fbx_filename = PathHelp::make_absolute(PathHelp::fullpath(filename), animation.fbx_filename);
		}
		desc.animations.push_back(animation);
	}

	for (const auto &json_attachment : json["attachment_points"].items())
	{
		ModelDescAttachmentPoint attachment;
		attachment.bone_name = json_attachment["bone_name"].to_string();
		attachment.name = json_attachment["name"].to_string();
		attachment.orientation = Quaternionf(json_attachment["orientation"]["x"].to_float(), json_attachment["orientation"]["y"].to_float(), json_attachment["orientation"]["z"].to_float(), json_attachment["orientation"]["w"].to_float());
		attachment.position = Vec3f(json_attachment["position"]["x"].to_float(), json_attachment["position"]["y"].to_float(), json_attachment["position"]["z"].to_float());
		if (!json_attachment["test_model"].is_undefined())
		{
			attachment.test_model = PathHelp::make_absolute(PathHelp::fullpath(filename), json_attachment["test_model"].to_string());
			attachment.test_scale = json_attachment["test_scale"].to_float();
		}
		desc.attachment_points.push_back(attachment);
	}

	if (!json["materials"].is_undefined())
	{
		for (const auto &json_material : json["materials"].items())
		{
			ModelDescMaterial material;
			if (!json_material["transparent"].is_undefined())
				material.transparent = json_material["transparent"].to_boolean();
			material.two_sided = json_material["two_sided"].to_boolean();
			material.alpha_test = json_material["alpha_test"].to_boolean();
			material.mesh_material = json_material["mesh_material"].to_string();
			desc.materials.push_back(material);
		}
	}

	if (!json["bones"].is_undefined())
	{
		for (const auto &json_bone : json["bones"].items())
		{
			ModelDescBone bone;
			bone.name = json_bone["name"].to_string();
			bone.mesh_bone = json_bone["mesh_bone"].to_string();
			bone.position = Vec3f(json_bone["position"]["x"].to_float(), json_bone["position"]["y"].to_float(), json_bone["position"]["z"].to_float());
			bone.rotation = Quaternionf(json_bone["rotation"]["w"].to_float(), json_bone["rotation"]["x"].to_float(), json_bone["rotation"]["y"].to_float(), json_bone["rotation"]["z"].to_float());
			bone.extents = Vec3f(json_bone["extents"]["x"].to_float(), json_bone["extents"]["y"].to_float(), json_bone["extents"]["z"].to_float());
			desc.bones.push_back(bone);
		}
	}

	for (const auto &json_emitter : json["emitters"].items())
	{
		ModelDescParticleEmitter emitter;
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

	return desc;
}

void ModelDesc::save(const std::string &filename)
{
	JsonValue json_animations = JsonValue::array();
	for (const auto &animation : animations)
	{
		JsonValue json_animation = JsonValue::object();
		json_animation["name"].set_string(animation.name);
		json_animation["start_frame"].set_number(animation.start_frame);
		json_animation["end_frame"].set_number(animation.end_frame);
		json_animation["play_speed"].set_number(animation.play_speed);
		json_animation["move_speed"].set_number(animation.move_speed);
		json_animation["loop"].set_boolean(animation.loop);
		json_animation["rarity"].set_number(animation.rarity);
		if (!animation.fbx_filename.empty())
			json_animation["fbx_filename"].set_string(PathHelp::make_relative(PathHelp::fullpath(filename), animation.fbx_filename));
		else
			json_animation["fbx_filename"].set_string(animation.fbx_filename);
		json_animations.items().push_back(json_animation);
	}

	JsonValue json_attachment_points = JsonValue::array();
	for (const auto &attachment : attachment_points)
	{
		JsonValue json_attachment = JsonValue::object();
		json_attachment["bone_name"].set_string(attachment.bone_name);
		json_attachment["name"].set_string(attachment.name);
		json_attachment["orientation"] = JsonValue::object();
		json_attachment["orientation"]["x"].set_number(attachment.orientation.x);
		json_attachment["orientation"]["y"].set_number(attachment.orientation.y);
		json_attachment["orientation"]["z"].set_number(attachment.orientation.z);
		json_attachment["orientation"]["w"].set_number(attachment.orientation.w);
		json_attachment["position"] = JsonValue::object();
		json_attachment["position"]["x"].set_number(attachment.position.x);
		json_attachment["position"]["y"].set_number(attachment.position.y);
		json_attachment["position"]["z"].set_number(attachment.position.z);
		json_attachment["test_model"].set_string(PathHelp::make_relative(PathHelp::fullpath(filename), attachment.test_model));
		json_attachment["test_scale"].set_number(attachment.test_scale);
		json_attachment_points.items().push_back(json_attachment);
	}

	JsonValue json_bones = JsonValue::array();
	for (const auto &bone : bones)
	{
		JsonValue json_bone = JsonValue::object();
		json_bone["name"].set_string(bone.name);
		json_bone["mesh_bone"].set_string(bone.mesh_bone);
		json_bone["position"] = JsonValue::object();
		json_bone["position"]["x"].set_number(bone.position.x);
		json_bone["position"]["y"].set_number(bone.position.y);
		json_bone["position"]["z"].set_number(bone.position.z);
		json_bone["rotation"] = JsonValue::object();
		json_bone["rotation"]["x"].set_number(bone.rotation.x);
		json_bone["rotation"]["y"].set_number(bone.rotation.y);
		json_bone["rotation"]["z"].set_number(bone.rotation.z);
		json_bone["rotation"]["w"].set_number(bone.rotation.w);
		json_bone["extents"] = JsonValue::object();
		json_bone["extents"]["x"].set_number(bone.extents.x);
		json_bone["extents"]["y"].set_number(bone.extents.y);
		json_bone["extents"]["z"].set_number(bone.extents.z);
		json_bones.items().push_back(json_bone);
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

	JsonValue json_emitters = JsonValue::array();

	JsonValue json = JsonValue::object();
	json["type"].set_string("fbx-model");
	json["version"].set_number(1);
	json["animations"] = json_animations;
	json["attachment_points"] = json_attachment_points;
	json["bones"] = json_bones;
	json["materials"] = json_materials;
	json["emitters"] = json_emitters;
	json["fbx_filename"].set_string(PathHelp::make_relative(PathHelp::fullpath(filename), fbx_filename));

	File::write_all_text(filename, json.to_json());
}
