
#include "precomp.h"
#include "demo_screen_controller.h"

using namespace uicore;

DemoScreenController::DemoScreenController()
{
	scene = Scene::create(scene_engine());
	scene->show_skybox_stars(false);
	scene->set_skybox_gradient(std::vector<Colorf> { Colorf::darkslategray, Colorf::darkslateblue });

	camera = SceneCamera::create(scene);
}

void DemoScreenController::update()
{
	scene_viewport()->set_camera(camera);
	scene_viewport()->render(gc());
}
