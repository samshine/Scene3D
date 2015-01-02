
#pragma once

#include "Model/EditorScene/editor_scene.h"

class EditScene : public EditorScene
{
public:
	EditScene();

	CharacterController *get_character_controller() { return 0; }

	void set_map_model(const std::string &map_model) override;
	void set_model_data(std::shared_ptr<clan::ModelData> model_data) override;
	void set_attachments(std::vector<SceneModelAttachment> attachments) override;

	void update(clan::Scene &scene, clan::GraphicContext &gc, clan::InputContext &ic, bool has_focus) override;

private:
	void setup_default_scene(clan::Scene &scene, clan::GraphicContext &gc);
	void update_model(clan::Scene &scene, clan::GraphicContext &gc);
	void update_map(clan::Scene &scene, clan::GraphicContext &gc);

	void play_animation(const std::string &name, bool instant);
	void play_transition(const std::string &anim1, const std::string &anim2, bool instant);
	std::string get_animation() const;

	std::shared_ptr<clan::ModelData> model_data;
	std::string current_animation = "default";

	clan::GameTime gametime;

	clan::SceneLight light1;
	clan::SceneLight light2;
	clan::SceneModel model1;
	clan::SceneObject object1;
	clan::SceneCamera camera;

	std::string map_model_filename;
	bool map_model_updated = true;
	clan::SceneModel map_model;
	clan::SceneObject map_object;

	std::vector<SceneModelAttachment> attachments;

	float dir = 0.0f;
	float up = 0.0f;
	float turn_speed = 90.0f;

	bool model_data_updated = true;

	bool button_strafe_left = false;
	bool button_strafe_right = false;
	bool button_jump = false;
	bool button_run_forward = false;
	bool button_run_backward = false;

	int last_input_x = 0;
	int last_input_y = 0;
};
