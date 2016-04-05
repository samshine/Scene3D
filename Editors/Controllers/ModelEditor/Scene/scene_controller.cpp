
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

	scene_viewport->clear_lines();

	float pivot_scale = 0.02f;
	Vec3f pivot_pos = scene_viewport->camera()->position() + Vec3f::normalize(pivot_pos - scene_viewport->camera()->position()) * 0.2f;

	// Move pivot:
	/*
	scene_viewport->draw_line(pivot_pos + Vec3f(0.1f, 0.0f, 0.0f) * pivot_scale, pivot_pos + Vec3f(0.8f, 0.0f, 0.0f) * pivot_scale, Vec3f(1.0f, 0.2f, 0.2f));
	scene_viewport->draw_line(pivot_pos + Vec3f(0.0f, 0.1f, 0.0f) * pivot_scale, pivot_pos + Vec3f(0.0f, 0.8f, 0.0f) * pivot_scale, Vec3f(0.2f, 1.0f, 0.2f));
	scene_viewport->draw_line(pivot_pos + Vec3f(0.0f, 0.0f, 0.1f) * pivot_scale, pivot_pos + Vec3f(0.0f, 0.0f, 0.8f) * pivot_scale, Vec3f(0.2f, 0.2f, 1.0f));
	scene_viewport->draw_line(pivot_pos + Vec3f(0.25f, 0.0f, 0.0f) * pivot_scale, pivot_pos + Vec3f(0.25f, 0.0f, 0.25f) * pivot_scale, Vec3f(1.0f, 0.2f, 0.2f));
	scene_viewport->draw_line(pivot_pos + Vec3f(0.25f, 0.0f, 0.0f) * pivot_scale, pivot_pos + Vec3f(0.25f, 0.25f, 0.0f) * pivot_scale, Vec3f(1.0f, 0.2f, 0.2f));
	scene_viewport->draw_line(pivot_pos + Vec3f(0.0f, 0.25f, 0.0f) * pivot_scale, pivot_pos + Vec3f(0.25f, 0.25f, 0.0f) * pivot_scale, Vec3f(0.2f, 1.0f, 0.2f));
	scene_viewport->draw_line(pivot_pos + Vec3f(0.0f, 0.25f, 0.0f) * pivot_scale, pivot_pos + Vec3f(0.0f, 0.25f, 0.25f) * pivot_scale, Vec3f(0.2f, 1.0f, 0.2f));
	scene_viewport->draw_line(pivot_pos + Vec3f(0.0f, 0.0f, 0.25f) * pivot_scale, pivot_pos + Vec3f(0.25f, 0.0f, 0.25f) * pivot_scale, Vec3f(0.2f, 0.2f, 1.0f));
	scene_viewport->draw_line(pivot_pos + Vec3f(0.0f, 0.0f, 0.25f) * pivot_scale, pivot_pos + Vec3f(0.0f, 0.25f, 0.25f) * pivot_scale, Vec3f(0.2f, 0.2f, 1.0f));
	*/

	// Rotate pivot:
	/*
	float s[73], t[73];
	for (int i = 0; i < 73; i++)
	{
		float angle = ((i * 5) % 360) * PI / 180.0f;
		s[i] = std::cos(angle);
		t[i] = std::sin(angle);
	}

	Vec3f eye_dir = pivot_pos - scene_viewport->camera()->position();
	for (int i = 0; i < 72; i++)
	{
		Vec3f rx0 = Vec3f(0.0f, 1.0f, 0.0f) * s[i] + Vec3f(0.0f, 0.0f, 1.0f) * t[i];
		Vec3f rx1 = Vec3f(0.0f, 1.0f, 0.0f) * s[i + 1] + Vec3f(0.0f, 0.0f, 1.0f) * t[i + 1];
		Vec3f ry0 = Vec3f(1.0f, 0.0f, 0.0f) * s[i] + Vec3f(0.0f, 0.0f, 1.0f) * t[i];
		Vec3f ry1 = Vec3f(1.0f, 0.0f, 0.0f) * s[i + 1] + Vec3f(0.0f, 0.0f, 1.0f) * t[i + 1];
		Vec3f rz0 = Vec3f(1.0f, 0.0f, 0.0f) * s[i] + Vec3f(0.0f, 1.0f, 0.0f) * t[i];
		Vec3f rz1 = Vec3f(1.0f, 0.0f, 0.0f) * s[i + 1] + Vec3f(0.0f, 1.0f, 0.0f) * t[i + 1];

		if (Vec3f::dot(rx0, eye_dir) < 0.0f)
			scene_viewport->draw_line(pivot_pos + rx0 * pivot_scale, pivot_pos + rx1 * pivot_scale, Vec3f(1.0f, 0.2f, 0.2f));
		if (Vec3f::dot(ry0, eye_dir) < 0.0f)
			scene_viewport->draw_line(pivot_pos + ry0 * pivot_scale, pivot_pos + ry1 * pivot_scale, Vec3f(0.2f, 0.1f, 0.2f));
		if (Vec3f::dot(rz0, eye_dir) < 0.0f)
			scene_viewport->draw_line(pivot_pos + rz0 * pivot_scale, pivot_pos + rz1 * pivot_scale, Vec3f(0.2f, 0.2f, 0.1f));
	}
	*/

	// Scale pivot:
	/*
	Vec3f eye_dir = pivot_pos - scene_viewport->camera()->position();
	float dx = (Vec3f::dot(Vec3f(1.0f, 0.0f, 0.0f), eye_dir) < 0.0f) ? 1.0f : -1.0f;
	float dy = (Vec3f::dot(Vec3f(0.0f, 1.0f, 0.0f), eye_dir) < 0.0f) ? 1.0f : -1.0f;
	float dz = (Vec3f::dot(Vec3f(0.0f, 0.0f, 1.0f), eye_dir) < 0.0f) ? 1.0f : -1.0f;
	Vec3f dv(dx, dy, dz);
	scene_viewport->draw_line(pivot_pos + Vec3f(0.0f, 0.0f, 0.0f) * dv * pivot_scale, pivot_pos + Vec3f(0.8f, 0.0f, 0.0f) * dv * pivot_scale, Vec3f(1.0f, 0.2f, 0.2f));
	scene_viewport->draw_line(pivot_pos + Vec3f(0.0f, 0.0f, 0.0f) * dv * pivot_scale, pivot_pos + Vec3f(0.0f, 0.8f, 0.0f) * dv * pivot_scale, Vec3f(0.2f, 1.0f, 0.2f));
	scene_viewport->draw_line(pivot_pos + Vec3f(0.0f, 0.0f, 0.0f) * dv * pivot_scale, pivot_pos + Vec3f(0.0f, 0.0f, 0.8f) * dv * pivot_scale, Vec3f(0.2f, 0.2f, 1.0f));
	scene_viewport->draw_line(pivot_pos + Vec3f(0.25f, 0.0f, 0.0f) * dv * pivot_scale, pivot_pos + Vec3f(0.0f, 0.0f, 0.25f) * dv * pivot_scale, Vec3f(1.0f, 1.2f, 0.2f));
	scene_viewport->draw_line(pivot_pos + Vec3f(0.0f, 0.0f, 0.25f) * dv * pivot_scale, pivot_pos + Vec3f(0.0f, 0.25f, 0.0f) * dv * pivot_scale, Vec3f(1.0f, 1.2f, 0.2f));
	scene_viewport->draw_line(pivot_pos + Vec3f(0.0f, 0.25f, 0.0f) * dv * pivot_scale, pivot_pos + Vec3f(0.25f, 0.0f, 0.0f) * dv * pivot_scale, Vec3f(1.0f, 1.2f, 0.2f));
	scene_viewport->draw_line(pivot_pos + Vec3f(0.4f, 0.0f, 0.0f) * dv * pivot_scale, pivot_pos + Vec3f(0.0f, 0.0f, 0.4f) * dv * pivot_scale, Vec3f(1.0f, 1.2f, 0.2f));
	scene_viewport->draw_line(pivot_pos + Vec3f(0.0f, 0.0f, 0.4f) * dv * pivot_scale, pivot_pos + Vec3f(0.0f, 0.4f, 0.0f) * dv * pivot_scale, Vec3f(1.0f, 1.2f, 0.2f));
	scene_viewport->draw_line(pivot_pos + Vec3f(0.0f, 0.4f, 0.0f) * dv * pivot_scale, pivot_pos + Vec3f(0.4f, 0.0f, 0.0f) * dv * pivot_scale, Vec3f(1.0f, 1.2f, 0.2f));
	*/

	// Bounding box:
	/*
	Vec3f bbox_min(-1.0f, 0.0f, -1.0f);
	Vec3f bbox_max( 1.0f, 2.0f,  1.0f);
	Vec3f corners[] =
	{
		Vec3f(bbox_min.x, bbox_min.y, bbox_min.z),
		Vec3f(bbox_max.x, bbox_min.y, bbox_min.z),
		Vec3f(bbox_max.x, bbox_min.y, bbox_max.z),
		Vec3f(bbox_min.x, bbox_min.y, bbox_max.z),
		Vec3f(bbox_min.x, bbox_max.y, bbox_min.z),
		Vec3f(bbox_max.x, bbox_max.y, bbox_min.z),
		Vec3f(bbox_max.x, bbox_max.y, bbox_max.z),
		Vec3f(bbox_min.x, bbox_max.y, bbox_max.z),
	};
	Vec4i pairs[] =
	{
		Vec4i(0, 3, 1, 4),
		Vec4i(1, 0, 2, 5),
		Vec4i(2, 1, 3, 6),
		Vec4i(3, 2, 0, 7),
		Vec4i(4, 5, 7, 0),
		Vec4i(5, 6, 4, 1),
		Vec4i(6, 7, 5, 2),
		Vec4i(7, 4, 6, 3)
	};
	for (const auto &pair : pairs)
	{
		scene_viewport->draw_line(corners[pair.x], mix(corners[pair.x], corners[pair.y], 0.20f), Vec3f(1.0f, 1.0f, 1.0f));
		scene_viewport->draw_line(corners[pair.x], mix(corners[pair.x], corners[pair.z], 0.20f), Vec3f(1.0f, 1.0f, 1.0f));
		scene_viewport->draw_line(corners[pair.x], mix(corners[pair.x], corners[pair.w], 0.20f), Vec3f(1.0f, 1.0f, 1.0f));
	}
	*/

	// Grid:
	for (int i = -10; i <= 10; i++)
	{
		auto color = (i % 10) == 0 ? Vec3f(0.2f, 0.2f, 0.2f) : Vec3f(0.05f, 0.05f, 0.05f);
		scene_viewport->draw_line(Vec3f(i * 0.1f, 0.0f, -1.0f), Vec3f(i * 0.1f, 0.0f, 1.0f), color);
		scene_viewport->draw_line(Vec3f(-1.0f, 0.0f, i * 0.1f), Vec3f(1.0f, 0.0f, i * 0.1f), color);
	}
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
