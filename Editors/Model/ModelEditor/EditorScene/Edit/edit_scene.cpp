
#include "precomp.h"
#include "edit_scene.h"

using namespace uicore;

EditScene::EditScene()
{
}

void EditScene::update(Scene &scene, GraphicContext &gc, DisplayWindow &ic, bool has_focus, const uicore::Vec2i &mouse_delta)
{
	setup_default_scene(scene, gc);

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

void EditScene::setup_default_scene(Scene &scene, GraphicContext &gc)
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

std::string EditScene::get_animation() const
{
	if (!object1.is_null())
		return object1.get_animation();
	else
		return current_animation;
}

void EditScene::play_animation(const std::string &name, bool instant)
{
	current_animation = name;
	if (!object1.is_null())
		object1.play_animation(current_animation, instant);
}

void EditScene::play_transition(const std::string &anim1, const std::string &anim2, bool instant)
{
	current_animation = anim2;
	if (!object1.is_null())
		object1.play_transition(anim1, anim2, instant);
}

void EditScene::set_map_model(const std::string &new_map_model)
{
	map_model_filename = new_map_model;
	map_model_updated = true;
}

void EditScene::set_model_data(std::shared_ptr<ModelData> new_model_data)
{
	model_data = new_model_data;
	model_data_updated = true;
}

void EditScene::set_attachments(std::vector<SceneModelAttachment> new_attachments)
{
	attachments = new_attachments;
	model_data_updated = true;
}

void EditScene::update_map(Scene &scene, GraphicContext &gc)
{
	if (!map_model_updated) return;
	map_model_updated = false;

	map_model = SceneModel();
	map_object = SceneObject();

	try
	{
		ModelDesc model_desc = ModelDesc::load(map_model_filename);

		FBXModel fbx_model(model_desc.fbx_filename);
		auto attachment_model_data = fbx_model.convert(model_desc);

		map_model = SceneModel(scene, attachment_model_data);
		map_object = SceneObject(scene, map_model);
	}
	catch (Exception &)
	{
	}
}

void EditScene::update_model(Scene &scene, GraphicContext &gc)
{
	if (!model_data_updated) return;
	model_data_updated = false;

	model1 = SceneModel();
	object1 = SceneObject();

	if (model_data)
	{
		model1 = SceneModel(scene, model_data);
		object1 = SceneObject(scene, model1, Vec3f(), Quaternionf(), Vec3f(1.0f));
		object1.play_animation(current_animation, true);

		for (auto &attachment : attachments)
		{
			try
			{
				ModelDesc model_desc = ModelDesc::load(attachment.model_name);

				FBXModel fbx_model(model_desc.fbx_filename);
				auto attachment_model_data = fbx_model.convert(model_desc);

				attachment.model = SceneModel(scene, attachment_model_data);
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
