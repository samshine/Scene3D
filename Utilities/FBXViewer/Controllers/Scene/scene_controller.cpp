
#include "precomp.h"
#include "scene_controller.h"
#include "Model/app_model.h"
#include "Views/Scene/scene_view.h"

using namespace clan;

SceneController::SceneController()
{
	view = std::make_shared<SceneView>();

	slots.connect(AppModel::instance()->sig_model_data_updated, [this]()
	{
		scene_view()->set_model_data(AppModel::instance()->model_data);
	});

	slots.connect(scene_view()->sig_key_press(), [this](KeyEvent &e)
	{
		if (e.key() == Key::left && !left_down)
		{
			left_down = true;
			update_anim();
		}
		else if (e.key() == Key::up && !up_down)
		{
			up_down = true;
			update_anim();
		}
	});

	slots.connect(scene_view()->sig_key_release(), [this](KeyEvent &e)
	{
		if (e.key() == Key::left && left_down)
		{
			left_down = false;
			update_anim();
		}
		else if (e.key() == Key::up && up_down)
		{
			up_down = false;
			update_anim();
		}
	});
}

void SceneController::update_anim()
{
	std::string cur_anim = scene_view()->get_animation();
	if (up_down && !left_down)
	{
		if (cur_anim == "strafe")
			scene_view()->play_transition("strafe-to-run", "run", false);
		else if (cur_anim != "run")
			scene_view()->play_animation("run", false);
	}
	else if (left_down)
	{
		if (cur_anim == "run")
			scene_view()->play_transition("run-to-strafe", "strafe", false);
		else if (cur_anim != "strafe")
			scene_view()->play_animation("strafe", false);
	}
	else
	{
		if (cur_anim != "default")
			scene_view()->play_animation("default", false);
	}
}

std::shared_ptr<SceneView> SceneController::scene_view()
{
	return std::static_pointer_cast<SceneView>(view);
}
