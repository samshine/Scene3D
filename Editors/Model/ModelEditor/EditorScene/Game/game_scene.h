
#pragma once

#include "Model/ModelEditor/EditorScene/editor_scene.h"
#include "character_controller.h"

class GameScene : public EditorScene
{
public:
	GameScene();

	CharacterController *get_character_controller() { return &character_controller; }

	void set_map_model(const std::string &map_model) override;
	void set_model_data(std::shared_ptr<clan::ModelData> model_data) override;
	void set_attachments(std::vector<SceneModelAttachment> attachments) override;

	void update(clan::Scene &scene, clan::GraphicContext &gc, clan::InputContext &ic, bool has_focus, const clan::Vec2i &mouse_delta) override;

private:
	void update_map(clan::Scene &scene, clan::GraphicContext &gc);
	void update_model(clan::Scene &scene, clan::GraphicContext &gc);
	void update_camera(clan::Scene &scene, clan::GraphicContext &gc);
	void update_input(clan::InputContext &ic, bool has_focus, const clan::Vec2i &mouse_delta);
	void update_character_controller();

	bool space_was_down = false;
	std::string last_anim = "default";

	clan::GameTime gametime;
	clan::Physics3DWorld collision_world;
	CharacterController character_controller;
	float dodge_cooldown = 0;
	float double_tap_left_elapsed = 1;
	float double_tap_right_elapsed = 1;
	float double_tap_up_elapsed = 1;
	float double_tap_down_elapsed = 1;
	bool was_down_left = false;
	bool was_down_right = false;
	bool was_down_up = false;
	bool was_down_down = false;

	std::string map_filename;
	bool map_updated = true;
	clan::SceneObject map_object;
	clan::Physics3DObject map_collision;

	bool model_updated = true;
	std::shared_ptr<clan::ModelData> model_data;
	clan::SceneObject model_object;
	std::vector<SceneModelAttachment> model_attachments;
};
