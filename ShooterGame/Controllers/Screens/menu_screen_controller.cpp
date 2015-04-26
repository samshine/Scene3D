
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
}
