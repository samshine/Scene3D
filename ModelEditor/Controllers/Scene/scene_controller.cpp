
#include "precomp.h"
#include "scene_controller.h"
#include "Model/app_model.h"
#include "Views/Scene/scene_view.h"

using namespace clan;

SceneController::SceneController()
{
	view = std::make_shared<SceneView>();

	slots.connect(AppModel::instance()->sig_model_data_updated, this, &SceneController::on_model_data_updated);
	slots.connect(AppModel::instance()->sig_map_model_updated, this, &SceneController::on_map_model_updated);
	slots.connect(scene_view()->sig_update_scene, this, &SceneController::update_scene);

	on_map_model_updated();
	on_model_data_updated();
}

std::shared_ptr<SceneView> SceneController::scene_view()
{
	return std::static_pointer_cast<SceneView>(view);
}

void SceneController::on_model_data_updated()
{
	set_model_data(AppModel::instance()->model_data);

	std::vector<SceneModelAttachment> attachments;
	for (const auto &fbx_attach : AppModel::instance()->desc.attachment_points)
	{
		attachments.push_back(SceneModelAttachment(fbx_attach.name, fbx_attach.test_model, fbx_attach.test_scale));
	}
	set_attachments(attachments);
}

void SceneController::on_map_model_updated()
{
	set_map_model(AppModel::instance()->map_model);
}

void SceneController::update_scene(Scene &scene, GraphicContext &gc, InputContext &ic)
{
	setup_default_scene(scene, gc);

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
		std::string cur_anim = get_animation();
		if (input_x < 0)
		{
			if (cur_anim != "strafe-left") play_animation("strafe-left", false);
		}
		else if (input_x > 0)
		{
			if (cur_anim != "strafe-right") play_animation("strafe-right", false);
		}
		else if (input_y > 0)
		{
			if (cur_anim != "forward") play_animation("forward", false);
		}
		else if (input_y < 0)
		{
			if (cur_anim != "backward") play_animation("backward", false);
		}
		else
		{
			if (cur_anim != "default") play_animation("default", false);
		}
	}

	last_input_x = input_x;
	last_input_y = input_y;

	update_map(scene, gc);
	update_model(scene, gc);

	camera.set_orientation(Quaternionf(up, dir, 0.0f, angle_degrees, order_YXZ));
	camera.set_position(camera.get_orientation().rotate_vector(Vec3f(0.0f, 10.0f, -25.0f)));

	gametime.update();

	if (!object1.is_null())
	{
		object1.update(gametime.get_time_elapsed());
		for (auto &attachment : attachments)
		{
			if (attachment.object.is_null())
				continue;

			Vec3f attach_pos, attach_scale;
			Quaternionf attach_orientation;
			object1.get_attachment_location(attachment.attachment_name, attach_pos, attach_orientation, attach_scale);
			attachment.object.set_position(attach_pos);
			attachment.object.set_orientation(attach_orientation);
			attachment.object.set_scale(attach_scale * attachment.model_scale);
			attachment.object.update(gametime.get_time_elapsed());
		}
	}
	scene.update(gc, gametime.get_time_elapsed());
}

void SceneController::setup_default_scene(Scene &scene, GraphicContext &gc)
{
	if (!light1.is_null())
		return;

	light1 = SceneLight(scene);
	light1.set_position(Vec3f(30.0f, 42.0f, -30.0f));
	light1.set_orientation(Quaternionf(45.0f, 315.0f, 0.0f, angle_degrees, order_YXZ));
	light1.set_type(SceneLight::type_spot);
	light1.set_attenuation_start(900.0f);
	light1.set_attenuation_end(1000.0f);
	light1.set_color(Vec3f(1.0f, 0.95f, 0.90f));
	light1.set_shadow_caster(true);
	light1.set_hotspot(10.0f);
	light1.set_falloff(45.0f);

	light2 = SceneLight(scene);
	light2.set_position(Vec3f(-100.0f, -100.0f, -100.0f));
	light2.set_type(SceneLight::type_omni);
	light2.set_attenuation_start(900.0f);
	light2.set_attenuation_end(1000.0f);
	light2.set_color(Vec3f(0.1f, 0.1f, 0.12f));

	camera = SceneCamera(scene);
	scene.set_camera(camera);

	gametime.reset();
}

std::string SceneController::get_animation() const
{
	if (!object1.is_null())
		return object1.get_animation();
	else
		return current_animation;
}

void SceneController::play_animation(const std::string &name, bool instant)
{
	current_animation = name;
	if (!object1.is_null())
		object1.play_animation(current_animation, instant);
}

void SceneController::play_transition(const std::string &anim1, const std::string &anim2, bool instant)
{
	current_animation = anim2;
	if (!object1.is_null())
		object1.play_transition(anim1, anim2, instant);
}

void SceneController::set_map_model(const std::string &new_map_model)
{
	map_model_filename = new_map_model;
	map_model_updated = true;
}

void SceneController::set_model_data(std::shared_ptr<ModelData> new_model_data)
{
	model_data = new_model_data;
	model_data_updated = true;
}

void SceneController::set_attachments(std::vector<SceneModelAttachment> new_attachments)
{
	attachments = new_attachments;
	model_data_updated = true;
}

void SceneController::update_map(clan::Scene &scene, GraphicContext &gc)
{
	if (!map_model_updated) return;
	map_model_updated = false;

	map_model = SceneModel();
	map_object = SceneObject();

	try
	{
		FBXModelDesc model_desc = FBXModelDesc::load(map_model_filename);

		FBXModel fbx_model(model_desc.fbx_filename);
		auto attachment_model_data = fbx_model.convert(model_desc);

		map_model = SceneModel(gc, scene, attachment_model_data);
		map_object = SceneObject(scene, map_model);
	}
	catch (Exception &)
	{
	}
}

void SceneController::update_model(clan::Scene &scene, GraphicContext &gc)
{
	if (!model_data_updated) return;
	model_data_updated = false;

	model1 = SceneModel();
	object1 = SceneObject();

	if (model_data)
	{
		model1 = SceneModel(gc, scene, model_data);
		object1 = SceneObject(scene, model1, Vec3f(), Quaternionf(), Vec3f(1.0f));
		object1.play_animation(current_animation, true);

		for (auto &attachment : attachments)
		{
			try
			{
				FBXModelDesc model_desc = FBXModelDesc::load(attachment.model_name);

				FBXModel fbx_model(model_desc.fbx_filename);
				auto attachment_model_data = fbx_model.convert(model_desc);

				attachment.model = SceneModel(gc, scene, attachment_model_data);
				attachment.object = SceneObject(scene, attachment.model, Vec3f(), Quaternionf(), Vec3f(attachment.model_scale));
			}
			catch (Exception &)
			{
			}
		}
	}
	else
	{
		model1 = SceneModel();
		object1 = SceneObject();
	}
}
