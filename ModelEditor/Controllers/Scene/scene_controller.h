
#pragma once

class SceneView;
class SceneModelAttachment;

class SceneController : public clan::ViewController
{
public:
	SceneController();

private:
	std::shared_ptr<SceneView> scene_view();

	void on_model_data_updated();
	void on_map_model_updated();
	void update_scene(clan::Scene &scene, clan::GraphicContext &gc, clan::InputContext &ic);

	void setup_default_scene(clan::Scene &scene, clan::GraphicContext &gc);
	void update_model(clan::Scene &scene, clan::GraphicContext &gc);
	void update_map(clan::Scene &scene, clan::GraphicContext &gc);

	void set_map_model(const std::string &map_model);
	void set_model_data(std::shared_ptr<clan::ModelData> model_data);
	void set_attachments(std::vector<SceneModelAttachment> attachments);

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

	clan::SlotContainer slots;
};

class SceneModelAttachment
{
public:
	SceneModelAttachment() { }
	SceneModelAttachment(std::string attachment_name, std::string model_name, float model_scale) : attachment_name(std::move(attachment_name)), model_name(std::move(model_name)), model_scale(model_scale) { }

	std::string attachment_name;
	std::string model_name;
	float model_scale = 1.0f;

	clan::SceneModel model;
	clan::SceneObject object;
};
