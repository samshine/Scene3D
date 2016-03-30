
#include "precomp.h"
#include "header_menu_view.h"

using namespace uicore;

HeaderMenuView::HeaderMenuView(const std::string &text, const std::string &icon, bool last)
{
	style()->set("flex-direction: row");
	style()->set("flex: none");
	style()->set("padding: 2px 0");
	style()->set("background: rgb(83,83,83)");

	button = std::make_shared<ButtonBaseView>();
	button->style()->set("flex: none");
	button->style()->set("margin: auto 0");
	button->style()->set("padding: 3px 10px");
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
	slots.connect(button->sig_pointer_release(), bind_member(this, &HeaderMenuView::button_clicked));
	add_child(button);

	menu = std::make_shared<HeaderMenuPopupController>(this, last);

	if (last)
		button->move_label_before_image();
}

void HeaderMenuView::button_clicked(PointerEvent &e)
{
	e.stop_propagation();
	WindowManager::present_popup(this, { 0.0f, 40.0f }, menu);
}

void HeaderMenuView::add_item(const std::string &text, std::function<void()> click)
{
	auto button = std::make_shared<ButtonBaseView>();
	button->style()->set("flex: none");
	button->style()->set("padding: 3px 10px");
	button->label()->set_text(Text::to_upper(text));
	button->label()->style()->set("font: 12px/18px 'Lato'");
	button->label()->style()->set("color: rgb(230,230,230)");
	slots.connect(button->sig_pointer_release(), [=](PointerEvent &e)
	{
		e.stop_propagation();
		style()->set("background: none");
		menu->dismiss();
		click();
	});
	menu->items->add_child(button);
}
