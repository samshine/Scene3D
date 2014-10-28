
#include "precomp.h"
#include "scene_view.h"
#include "viewer_scene_cache.h"
#include "../../../Model/app_model.h"

using namespace clan;

SceneView::SceneView()
{
	style.set_layout_hbox();
	style.set_flex(1.0f, 1.0f);
	style.set_background(Colorf(236, 240, 243));
	style.set_padding(5.0f);

	set_focus_policy(FocusPolicy::accept);

	slots.connect(AppModel::instance()->sig_model_data_updated(), [this]()
	{
		model_data_updated = true;
		set_needs_render();
	});

	slots.connect(sig_pointer_press(), [this](PointerEvent &e)
	{
		set_focus();
	});

	slots.connect(sig_key_press(), [this](KeyEvent &e)
	{
		if (e.key() == Key::left && e.repeat_count() == 0)
		{
			keys_down++;

			current_animation = "left";
			if (!object1.is_null())
				object1.play_animation(current_animation);
		}
		else if (e.key() == Key::right && e.repeat_count() == 0)
		{
			keys_down++;

			current_animation = "right";
			if (!object1.is_null())
				object1.play_animation(current_animation);
		}
		else if (e.key() == Key::up && e.repeat_count() == 0)
		{
			keys_down++;

			current_animation = "up";
			if (!object1.is_null())
				object1.play_animation(current_animation);
		}
		else if (e.key() == Key::down && e.repeat_count() == 0)
		{
			keys_down++;

			current_animation = "down";
			if (!object1.is_null())
				object1.play_animation(current_animation);
		}
	});

	slots.connect(sig_key_release(), [this](KeyEvent &e)
	{
		if (e.key() == Key::left || e.key() == Key::right || e.key() == Key::up || e.key() == Key::down)
		{
			keys_down--;
			if (keys_down == 0)
			{
				current_animation = "default";
				if (!object1.is_null())
					object1.play_animation(current_animation);
			}
		}
	});

	/*timer = Timer();
	timer.func_expired() = [this]()
	{
		set_needs_render();
	};
	timer.start(10, true);*/
}

void SceneView::render_content(Canvas &canvas)
{
	return;
	Pointf viewport_pos = Vec2f(canvas.get_transform() * Vec4f(0.0f, 0.0f, 0.0f, 1.0f));
	Sizef viewport_size = geometry().content.get_size();

	clan::GraphicContext gc = canvas.get_gc();

	setup_scene(gc);
	update_model(gc);

	camera.set_orientation(clan::Quaternionf(up, dir, 0.0f, clan::angle_degrees, clan::order_YXZ));
	camera.set_position(camera.get_orientation().rotate_vector(clan::Vec3f(0.0f, 15.0f, -25.0f)));

	gametime.update();

	if (!object1.is_null()) object1.update(gametime.get_time_elapsed());
	scene.update(gc, gametime.get_time_elapsed());

	scene.set_viewport(clan::RectfPS(viewport_pos.x, viewport_pos.y, viewport_size.width, viewport_size.height));
	scene.render(gc);

	gc.set_viewport(gc.get_size());
}

void SceneView::update_model(clan::GraphicContext &gc)
{
	if (!model_data_updated) return;
	model_data_updated = false;

	model1 = clan::SceneModel();
	object1 = clan::SceneObject();

	if (AppModel::instance()->model_data())
	{
		model1 = clan::SceneModel(gc, scene, AppModel::instance()->model_data());
		object1 = clan::SceneObject(scene, model1, clan::Vec3f(), clan::Quaternionf(), clan::Vec3f(1.0f));
		object1.play_animation(current_animation);
	}
}

void SceneView::setup_scene(clan::GraphicContext &gc)
{
	if (!scene.is_null()) return;

	resources = clan::ResourceManager();
	clan::SceneCache::set(resources, std::shared_ptr<clan::SceneCache>(new ViewerSceneCache()));

	scene = clan::Scene(gc, resources, "Resources/Scene3D");

	scene.show_skybox_stars(false);
	std::vector<clan::Colorf> gradient;
	gradient.push_back(clan::Colorf::gray70);
	gradient.push_back(clan::Colorf::gray70);
	gradient.push_back(clan::Colorf::gray70);
	gradient.push_back(clan::Colorf::gray70);
	gradient.push_back(clan::Colorf::gray60);
	gradient.push_back(clan::Colorf::gray50);
	gradient.push_back(clan::Colorf::gray40);
	gradient.push_back(clan::Colorf::gray30);
	scene.set_skybox_gradient(gc, gradient);

	light1 = clan::SceneLight(scene);
	light1.set_position(clan::Vec3f(100.0f, 100.0f, 100.0f));
	light1.set_type(clan::SceneLight::type_omni);
	light1.set_attenuation_start(900.0f);
	light1.set_attenuation_end(1000.0f);
	light1.set_color(clan::Vec3f(1.0f));

	light2 = clan::SceneLight(scene);
	light2.set_position(clan::Vec3f(-100.0f, -100.0f, -100.0f));
	light2.set_type(clan::SceneLight::type_omni);
	light2.set_attenuation_start(900.0f);
	light2.set_attenuation_end(1000.0f);
	light2.set_color(clan::Vec3f(0.6f));

	camera = clan::SceneCamera(scene);
	scene.set_camera(camera);

	gametime.reset();
}
