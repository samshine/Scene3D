
#pragma once

class SceneView;

class SceneController : public clan::ViewController
{
public:
	SceneController();

private:
	std::shared_ptr<SceneView> scene_view();

	void model_data_updated();
	void map_model_updated();
	void update_scene(clan::Scene &scene, clan::GraphicContext &gc, clan::InputContext &ic);

	bool button_strafe_left = false;
	bool button_strafe_right = false;
	bool button_jump = false;
	bool button_run_forward = false;
	bool button_run_backward = false;

	int last_input_x = 0;
	int last_input_y = 0;

	clan::SlotContainer slots;
};
