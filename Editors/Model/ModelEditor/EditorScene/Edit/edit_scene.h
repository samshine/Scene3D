
#pragma once

#include "Model/ModelEditor/EditorScene/editor_scene.h"

class EditScene : public EditorScene
{
public:
	EditScene();

	CharacterController *get_character_controller() { return 0; }

	void set_map_model(const std::string &map_model) override;
	void set_model_data(std::shared_ptr<ModelData> model_data) override;
	void set_attachments(std::vector<SceneModelAttachment> attachments) override;

	void update(Scene &scene, const uicore::GraphicContextPtr &gc, const uicore::DisplayWindowPtr &window, bool has_focus, const uicore::Vec2i &mouse_delta) override;

private:
	void setup_default_scene(Scene &scene, const uicore::GraphicContextPtr &gc);
	void update_model(Scene &scene, const uicore::GraphicContextPtr &gc);
	void update_map(Scene &scene, const uicore::GraphicContextPtr &gc);

	void play_animation(const std::string &name, bool instant);
	void play_transition(const std::string &anim1, const std::string &anim2, bool instant);
	std::string get_animation() const;

	std::shared_ptr<ModelData> model_data;
	std::string current_animation = "default";

	uicore::GameTime gametime;

	SceneLight light1;
	SceneLight light2;
	SceneModel model1;
	SceneObject object1;
	SceneCamera camera;

	std::string map_model_filename;
	bool map_model_updated = true;
	SceneModel map_model;
	SceneObject map_object;

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
