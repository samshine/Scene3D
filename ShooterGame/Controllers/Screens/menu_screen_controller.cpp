
#include "precomp.h"
#include "menu_screen_controller.h"
#include "game_screen_controller.h"

using namespace uicore;

MenuScreenController::MenuScreenController(const CanvasPtr &canvas) : ScreenViewController(canvas)
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
		auto item = std::make_shared<ButtonView>();
		item->label()->style()->set("font: 30px/40px 'LuckiestGuy'; color: #ccc");
		item->label()->set_text_alignment(TextAlignment::center);
		item->label()->set_text(str);
		item->func_clicked() = [this]()
		{
			Screen::controller() = std::make_shared<GameScreenController>(texture_view->get_canvas());
		};
		menu_box->add_subview(item);
	}

	texture_view->add_subview(menu_box);

	GraphicContextPtr gc = canvas->gc();

	scene = Scene(Screen::scene_cache());
	scene.set_camera(SceneCamera(scene));
	scene.get_camera().set_position(Vec3f(0.0f, 1.8f, -3.0f));
	//scene.get_camera().set_orientation(Quaternionf(0.0f, 180.0f, 0.0f, angle_degrees, order_YXZ));

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

	SceneModel model(scene, "Levels/Liandri/liandri.cmodel");
	map_object = SceneObject(scene, model);
}

void MenuScreenController::update_desktop(const uicore::CanvasPtr &canvas, const uicore::DisplayWindowPtr &ic, const uicore::Vec2i &mouse_delta)
{
	game_time.update();

	t = std::fmod(t + game_time.get_time_elapsed() * 0.01f, 2.0f);

	float t2 = t > 1.0f ? 2.0f - t : t;
	scene.get_camera().set_position(Vec3f(-12.0f, 2.5f + 1.8f, -13.0f - 3.0f * t2));

	scene.update(canvas->gc(), game_time.get_time_elapsed());

	render_scene(canvas, scene);
}
