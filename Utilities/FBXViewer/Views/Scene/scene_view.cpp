
#include "precomp.h"
#include "scene_view.h"
#include "viewer_scene_cache.h"

using namespace clan;

SceneView::SceneView()
{
	box_style.set_layout_hbox();
	box_style.set_flex(1.0f, 1.0f);

	set_focus_policy(FocusPolicy::accept);

	slots.connect(sig_pointer_press(), [this](PointerEvent &e)
	{
		set_focus();
	});

	timer = Timer();
	timer.func_expired() = [this]()
	{
		set_needs_render();
	};
}

std::string SceneView::get_animation() const
{
	if (!object1.is_null())
		return object1.get_animation();
	else
		return current_animation;
}

void SceneView::play_animation(const std::string &name, bool instant)
{
	current_animation = name;
	if (!object1.is_null())
		object1.play_animation(current_animation, instant);
}

void SceneView::play_transition(const std::string &anim1, const std::string &anim2, bool instant)
{
	current_animation = anim2;
	if (!object1.is_null())
		object1.play_transition(anim1, anim2, instant);
}

void SceneView::set_model_data(std::shared_ptr<clan::ModelData> new_model_data)
{
	model_data = new_model_data;
	model_data_updated = true;
	set_needs_render();
}

void SceneView::render_content(Canvas &canvas)
{
	Pointf viewport_pos = Vec2f(canvas.get_transform() * Vec4f(0.0f, 0.0f, 0.0f, 1.0f));
	Sizef viewport_size = geometry().content.get_size();

	canvas.flush();
	clan::GraphicContext gc = canvas.get_gc();
	clan::InputContext ic;
	if (dynamic_cast<WindowView*>(root_view())) ic = static_cast<WindowView*>(root_view())->get_display_window().get_ic();

	setup_scene(gc);
	sig_update_scene(scene, gc, ic);
	update_model(gc);

	camera.set_orientation(clan::Quaternionf(up, dir, 0.0f, clan::angle_degrees, clan::order_YXZ));
	camera.set_position(camera.get_orientation().rotate_vector(clan::Vec3f(0.0f, 15.0f, -25.0f)));

	gametime.update();

	if (!object1.is_null()) object1.update(gametime.get_time_elapsed());
	scene.update(gc, gametime.get_time_elapsed());

	scene.set_viewport(clan::RectfPS(viewport_pos.x, viewport_pos.y, viewport_size.width, viewport_size.height));
	scene.render(gc);

	gc.set_viewport(gc.get_size());

	timer.start(10, false);
}

void SceneView::update_model(clan::GraphicContext &gc)
{
	if (!model_data_updated) return;
	model_data_updated = false;

	model1 = clan::SceneModel();
	object1 = clan::SceneObject();

	if (model_data)
	{
		model1 = clan::SceneModel(gc, scene, model_data);
		object1 = clan::SceneObject(scene, model1, clan::Vec3f(), clan::Quaternionf(), clan::Vec3f(1.0f));
		object1.play_animation(current_animation, true);
	}
	else
	{
		model1 = clan::SceneModel();
		object1 = clan::SceneObject();
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
