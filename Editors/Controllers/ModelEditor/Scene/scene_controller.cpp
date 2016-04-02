
#include "precomp.h"
#include "scene_controller.h"
#include "Model/ModelEditor/model_app_model.h"
#include "Views/Scene/scene_view.h"
#include "Views/Scene/scene_rotate_action.h"
#include "Views/Scene/scene_move_action.h"

using namespace uicore;

SceneController::SceneController()
{
	view = std::make_shared<SceneView>();
	set_root_view(view);

	rotate_action->func_mouse_move = [=](const uicore::Vec2i &mouse_delta)
	{
		rotation.y = std::fmod(rotation.y + mouse_delta.x * mouse_speed_x, 360.0f);
		rotation.x = clamp(rotation.x + mouse_delta.y * mouse_speed_y, -90.0f, 90.0f);
		view->set_needs_render();
	};
	view->add_action(rotate_action);

	auto move_action = view->add_action<SceneMoveAction>();
	move_action->track_keys({ Key::a, Key::q, Key::d, Key::w, Key::z, Key::s, Key::lshift, Key::space, Key::lcontrol });
	move_action->func_key_repeat = [=](std::map<Key, bool> &keys, float time_elapsed)
	{
		if (view->has_focus())
		{
			Vec3f thrust;
			if (keys[Key::a] || keys[Key::q])
				thrust.x -= 1.0f;
			if (keys[Key::d])
				thrust.x += 1.0f;
			if (keys[Key::w] || keys[Key::z])
				thrust.z += 1.0f;
			if (keys[Key::s])
				thrust.z -= 1.0f;
			if (keys[Key::lshift] || keys[Key::space])
				thrust.y += 1.0f;
			if (keys[Key::lcontrol])
				thrust.y -= 1.0f;

			Quaternionf camera_quaternion(rotation.x, rotation.y, rotation.z, angle_degrees, order_YXZ);
			position += camera_quaternion.rotate_vector(thrust) * time_elapsed * move_speed;
		}

		view->set_needs_render();
	};

	view->set_animate();

	slots.connect(ModelAppModel::instance()->sig_model_data_updated, this, &SceneController::model_data_updated);
	slots.connect(ModelAppModel::instance()->sig_map_model_updated, this, &SceneController::map_model_updated);
	slots.connect(view->sig_update_scene, this, &SceneController::update_scene);

	slots.connect(view->sig_key_press(), [this](KeyEvent *e)
	{
		if ((int)e->key() >= (int)Key::key_0 && (int)e->key() <= (int)Key::key_9)
		{
			const auto &animations = ModelAppModel::instance()->desc.animations;
			int anim_select = (int)e->key() - (int)Key::key_0;
			if (anim_select < (int)animations.size())
			{
				const auto &animation = animations[anim_select];
				current_animation = animation.name;
				if (object1)
					object1->play_animation(current_animation, false);
			}
			e->stop_propagation();
		}
	});

	setup_scene();

	map_model_updated();
	model_data_updated();
}

void SceneController::setup_scene()
{
	if (scene) return;

	scene = Scene::create(SceneView::engine());
	scene->show_skybox_stars(false);
	std::vector<Colorf> gradient;
	gradient.push_back(Colorf(40 / 255.0f, 40 / 255.0f, 40 / 255.0f));
	for (auto &g : gradient)
	{
		g.r = std::pow(g.r, 2.2f);
		g.g = std::pow(g.g, 2.2f);
		g.b = std::pow(g.b, 2.2f);
	}
	scene->set_skybox_gradient(gradient);

	light1 = SceneLight::create(scene);
	light1->set_position(Vec3f(30.0f, 42.0f, -30.0f));
	light1->set_orientation(Quaternionf(45.0f, 315.0f, 0.0f, angle_degrees, order_YXZ));
	light1->set_type(SceneLight::type_spot);
	light1->set_attenuation_start(900.0f);
	light1->set_attenuation_end(1000.0f);
	light1->set_color(Vec3f(1.0f, 0.95f, 0.90f));
	light1->set_shadow_caster(true);
	light1->set_hotspot(10.0f);
	light1->set_falloff(45.0f);

	light2 = SceneLight::create(scene);
	light2->set_position(Vec3f(-100.0f, -100.0f, -100.0f));
	light2->set_type(SceneLight::type_omni);
	light2->set_attenuation_start(900.0f);
	light2->set_attenuation_end(1000.0f);
	light2->set_color(Vec3f(0.1f, 0.1f, 0.12f));

	view->viewport()->set_camera(SceneCamera::create(scene));
}

void SceneController::update_scene(const SceneViewportPtr &scene_viewport, const uicore::GraphicContextPtr &gc, const uicore::DisplayWindowPtr &window)
{
	gametime.update();

	Quaternionf camera_quaternion(rotation.x, rotation.y, rotation.z, angle_degrees, order_YXZ);

	scene_viewport->camera()->set_position(position);
	scene_viewport->camera()->set_orientation(camera_quaternion);

	scene_viewport->update(gc, gametime.time_elapsed());
	if (object1)
		object1->update(gametime.time_elapsed());
	for (auto &attachment : attachments)
		attachment->update(gametime.time_elapsed());
}

void SceneController::model_data_updated()
{
	model1 = nullptr;
	object1 = nullptr;
	attachments.clear();

	auto model_data = ModelAppModel::instance()->model_data;
	if (model_data)
	{
		model1 = SceneModel::create(scene, model_data);
		object1 = SceneObject::create(scene, model1, Vec3f(), Quaternionf(), Vec3f(1.0f));
		object1->play_animation(current_animation, true);

		for (auto &attachment : ModelAppModel::instance()->desc.attachment_points)
		{
			try
			{
				ModelDesc model_desc = ModelDesc::load(attachment.name);

				auto fbx_model = FBXModel::load(model_desc.fbx_filename);
				auto attachment_model_data = fbx_model->convert(model_desc);

				auto model = SceneModel::create(scene, attachment_model_data);
				attachments.push_back(SceneObject::create(scene, model, Vec3f(), Quaternionf(), Vec3f(attachment.test_scale)));
			}
			catch (Exception &)
			{
			}
		}
	}
}

void SceneController::map_model_updated()
{
}
