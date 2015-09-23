
#include "precomp.h"
#include "header_menu_view.h"

using namespace uicore;

HeaderMenuView::HeaderMenuView(const std::string &text, const std::string &icon, bool last)
{
	style()->set("flex-direction: row");
	style()->set("flex: none");
	style()->set("padding: 2px 0");

	button = std::make_shared<ButtonView>();
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
	button->label()->set_text(StringHelp::text_to_upper(text));
	button->label()->style()->set("font: 12px/18px 'Lato'");
	button->label()->style()->set("color: white");
	slots.connect(button->sig_pointer_release(), bind_member(this, &HeaderMenuView::button_clicked));
	add_subview(button);

	items = std::make_shared<PopupView>();
	items->style()->set("flex-direction: column");
	items->style()->set("position: absolute");
	items->style()->set("top: 40px");
	items->style()->set("width: 175px");
	items->style()->set("background: rgb(15,50,77)");
	items->style()->set("margin: 0 10px 10px 0");
	items->style()->set("box-shadow: 5px 5px 5px rgba(0,0,0,0.2)");
	items->set_hidden(true);
	add_subview(items);

	if (last)
	{
		button->move_label_before_image();
		items->style()->set("right: 0");
	}
	else
	{
		items->style()->set("left: 0");
	}
}

void HeaderMenuView::button_clicked(PointerEvent &e)
{
	e.stop_propagation();
	items->set_hidden(!items->hidden());
	if (!items->hidden())
		style()->set("background: rgb(15,50,77)");
	else
		style()->set("background: none");
}

void HeaderMenuView::add_item(const std::string &text, std::function<void()> click)
{
	auto button = std::make_shared<ButtonView>();
	button->style()->set("flex: none");
	button->style()->set("padding: 3px 10px");
	button->label()->set_text(StringHelp::text_to_upper(text));
	button->label()->style()->set("font: 12px/18px 'Lato'");
	button->label()->style()->set("color: white");
	slots.connect(button->sig_pointer_release(), [=](PointerEvent &e)
	{
		e.stop_propagation();
		style()->set("background: none");
		items->set_hidden(true);
		click();
	});
	items->add_subview(button);
}
