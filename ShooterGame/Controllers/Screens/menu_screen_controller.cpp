
#include "precomp.h"
#include "menu_screen_controller.h"

using namespace clan;

MenuScreenController::MenuScreenController(Canvas &canvas) : ScreenViewController(canvas)
{
	auto menu_box = std::make_shared<View>();

	menu_box->style()->set("width: 500px;");
	menu_box->style()->set("position: absolute; bottom: 150px; right: 150px");
	menu_box->style()->set("border-image: url('UI/window.png') 21 33 27 27 fill repeat");
	menu_box->style()->set("border-style: solid; border-width: 21px 33px 27px 27px");

	menu_box->style()->set("flex-direction: column");
	menu_box->style()->set("padding: 20px 0");

	auto title = std::make_shared<LabelView>();
	title->style()->set("font: 50px/65px 'LuckiestGuy'; color: #ddd; margin-bottom: 15px");
	title->set_text_alignment(TextAlignment::center);
	title->set_text("Main Menu");
	menu_box->add_subview(title);

	for (auto &str : { "New Game", "Join Game", "Host Game", "Options", "Quit"})
	{
		auto item = std::make_shared<LabelView>();
		item->style()->set("font: 30px/40px 'LuckiestGuy'; color: #ccc");
		item->set_text_alignment(TextAlignment::center);
		item->set_text(str);
		menu_box->add_subview(item);
	}

	view->add_subview(menu_box);

	GraphicContext gc = canvas.get_gc();

	scene = Scene(gc, UIThread::get_resources(), "../Resources/Scene3D");
	scene.set_camera(SceneCamera(scene));

	scene.show_skybox_stars(false);
	std::vector<Colorf> gradient;
	gradient.push_back(Colorf(236 * 5 / 10, 240 * 5 / 10, 243 * 5 / 10));
	gradient.push_back(Colorf(236 * 5 / 10, 240 * 5 / 10, 243 * 5 / 10));
	gradient.push_back(Colorf(236 * 5 / 10, 240 * 5 / 10, 243 * 5 / 10));
	gradient.push_back(Colorf(236 * 5 / 10, 240 * 5 / 10, 243 * 5 / 10));
	gradient.push_back(Colorf(236 * 5 / 10, 240 * 5 / 10, 243 * 5 / 10));
	gradient.push_back(Colorf(236 * 5 / 10, 240 * 5 / 10, 243 * 5 / 10));
	gradient.push_back(Colorf(236 * 6 / 10, 240 * 6 / 10, 243 * 6 / 10));
	gradient.push_back(Colorf(236 * 7 / 10, 240 * 7 / 10, 243 * 7 / 10));
	gradient.push_back(Colorf(236 * 8 / 10, 240 * 8 / 10, 243 * 8 / 10));
	for (auto &g : gradient)
	{
		g.r = std::pow(g.r, 2.2f);
		g.g = std::pow(g.g, 2.2f);
		g.b = std::pow(g.b, 2.2f);
	}
	scene.set_skybox_gradient(gc, gradient);

	SceneModel model(gc, scene, "Map3/map3.cmodel");
	map_object = SceneObject(scene, model);
}

void MenuScreenController::update_desktop(clan::Canvas &canvas)
{
	canvas.flush();

	GraphicContext gc = canvas.get_gc();

	Pointf viewport_pos = Vec2f(canvas.get_transform() * Vec4f(0.0f, 0.0f, 0.0f, 1.0f));
	Sizef viewport_size = gc.get_size();
	Size viewport_size_i = Size(viewport_size) * 2;

	scene.update(gc, 0.0f/*gametime.get_time_elapsed()*/);

	scene.set_viewport(viewport_size_i);
	scene.render(gc);

	gc.set_viewport(gc.get_size());
}
