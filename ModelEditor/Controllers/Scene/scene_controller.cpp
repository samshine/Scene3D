
#include "precomp.h"
#include "scene_controller.h"
#include "Model/app_model.h"
#include "Views/Scene/scene_view.h"

using namespace clan;

SceneController::SceneController()
{
	view = std::make_shared<SceneView>();

	slots.connect(AppModel::instance()->sig_model_data_updated, this, &SceneController::model_data_updated);
	slots.connect(AppModel::instance()->sig_map_model_updated, this, &SceneController::map_model_updated);
	slots.connect(scene_view()->sig_update_scene, this, &SceneController::update_scene);

	map_model_updated();
	model_data_updated();
}

void SceneController::model_data_updated()
{
	scene_view()->set_model_data(AppModel::instance()->model_data);

	std::vector<SceneViewAttachment> attachments;
	for (const auto &fbx_attach : AppModel::instance()->desc.attachment_points)
	{
		attachments.push_back(SceneViewAttachment(fbx_attach.name, fbx_attach.test_model, fbx_attach.test_scale));
	}
	scene_view()->set_attachments(attachments);
}

void SceneController::map_model_updated()
{
	scene_view()->set_map_model(AppModel::instance()->map_model);
}

void SceneController::update_scene(Scene &scene, GraphicContext &gc, InputContext &ic)
{
	bool has_focus = view.get() == view->focus_view();
	auto keyboard = ic.get_keyboard();

	button_strafe_left = has_focus && keyboard.get_keycode(keycode_left);
	button_strafe_right = has_focus && keyboard.get_keycode(keycode_right);
	button_jump = has_focus && keyboard.get_keycode(keycode_space);
	button_run_forward = has_focus && keyboard.get_keycode(keycode_up);
	button_run_backward = has_focus && keyboard.get_keycode(keycode_down);

	int input_x = button_strafe_left ? -1 : button_strafe_right ? 1 : 0;
	int input_y = button_run_forward ? 1 : button_run_backward ? -1 : 0;

	if (last_input_x != input_x || last_input_y != input_y)
	{
		std::string cur_anim = scene_view()->get_animation();
		if (input_x < 0)
		{
			if (cur_anim != "strafe-left") scene_view()->play_animation("strafe-left", false);
		}
		else if (input_x > 0)
		{
			if (cur_anim != "strafe-right") scene_view()->play_animation("strafe-right", false);
		}
		else if (input_y > 0)
		{
			if (cur_anim != "forward") scene_view()->play_animation("forward", false);
		}
		else if (input_y < 0)
		{
			if (cur_anim != "backward") scene_view()->play_animation("backward", false);
		}
		else
		{
			if (cur_anim != "default") scene_view()->play_animation("default", false);
		}
	}

	last_input_x = input_x;
	last_input_y = input_y;
}

std::shared_ptr<SceneView> SceneController::scene_view()
{
	return std::static_pointer_cast<SceneView>(view);
}
