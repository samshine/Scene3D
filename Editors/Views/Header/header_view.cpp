
#include "precomp.h"
#include "header_view.h"
#include "header_menu_view.h"

using namespace uicore;

HeaderView::HeaderView()
{
	style()->set(R"(
		flex-direction: row;
		flex: none;
		background: rgb(83,83,83);
		border-bottom: 1px solid rgb(159,184,194);
		)");

	left_buttons = std::make_shared<View>();
	left_buttons->style()->set("flex: none");
	left_buttons->style()->set("flex-direction: row");
	add_child(left_buttons);

	auto spacer = std::make_shared<View>();
	spacer->style()->set("flex: auto");
	add_child(spacer);

	right_buttons = std::make_shared<View>();
	right_buttons->style()->set("flex: none");
	right_buttons->style()->set("flex-direction: row");
	add_child(right_buttons);
}

void HeaderView::add_left_button(const std::string &text, std::function<void()> click)
{
	create_button(text, std::string(), click, true);
}

void HeaderView::add_left_button(const std::string &text, const std::string &icon, std::function<void()> click)
{
	create_button(text, icon, click, true);
}

void HeaderView::add_right_button(const std::string &text, std::function<void()> click)
{
	create_button(text, std::string(), click, false);
}

void HeaderView::add_right_button(const std::string &text, const std::string &icon, std::function<void()> click, bool last)
{
	create_button(text, icon, click, false, last);
}

std::shared_ptr<HeaderMenuView> HeaderView::add_left_menu(const std::string &text, const std::string &icon)
{
	auto menu = std::make_shared<HeaderMenuView>(text, icon, false);
	left_buttons->add_child(menu);
	return menu;
}

std::shared_ptr<HeaderMenuView> HeaderView::add_right_menu(const std::string &text, const std::string &icon, bool last)
{
	auto menu = std::make_shared<HeaderMenuView>(text, icon, last);
	right_buttons->add_child(menu);
	return menu;
}

void HeaderView::create_button(const std::string &text, const std::string &icon, std::function<void()> click, bool left, bool last)
{
	auto button = std::make_shared<ButtonBaseView>();
	button->style()->set("flex: none");
	button->style()->set("margin: auto 0");
	button->style()->set("padding: 5px 10px");
	if (!icon.empty())
	{
		if (!last)
			button->image_view()->style()->set("margin-right: 5px");
		else
			button->image_view()->style()->set("margin-left: 5px");
		button->image_view()->set_image(ImageSource::from_resource(icon));
	}
	button->label()->set_text(Text::to_upper(text));
	button->label()->style()->set("font: 12px/18px 'Lato'");
	button->label()->style()->set("color: rgb(230,230,230)");
	slots.connect(button->sig_pointer_release(), [=](PointerEvent &e) { click(); e.stop_propagation(); });
	if (left)
		left_buttons->add_child(button);
	else
		right_buttons->add_child(button);

	if (last)
		button->move_label_before_image();
}
