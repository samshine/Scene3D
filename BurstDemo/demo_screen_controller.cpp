
#include "precomp.h"
#include "demo_screen_controller.h"

using namespace uicore;

DemoScreenController::DemoScreenController()
{
	scene = Scene::create(scene_engine());
	scene->show_skybox_stars(false);
	scene->set_skybox_gradient(std::vector<Colorf> { Colorf::darkslategray, Colorf::darkslateblue });

	camera = SceneCamera::create(scene);

	box = SceneObject::create(scene, SceneModel::create(scene, ModelData::create_box(Vec3f{ 1.0f, 1.0f, 1.0f })));
	box->set_position(Vec3f{ 0.0f, 0.0f, 10.0f });

	light1 = SceneLight::create(scene);
	light1->set_position(Vec3f{ 10.0f, 10.0f, 10.0f });

	light2 = SceneLight::create(scene);
	light2->set_position(Vec3f{ -10.0f, -10.0f, -10.0f });
}

void DemoScreenController::update()
{
	float rotate = game_time().time_elapsed() * 100.0f;
	box->set_orientation(box->orientation() * Quaternionf(rotate, rotate, rotate, angle_degrees, order_XYZ));

	scene_viewport()->set_camera(camera);
	scene_viewport()->render(gc());
}
