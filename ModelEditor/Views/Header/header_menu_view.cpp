
#include "precomp.h"
#include "header_menu_view.h"

using namespace clan;

HeaderMenuView::HeaderMenuView(const std::string &text, const std::string &icon, bool last)
{
	box_style.set_layout_hbox();
	box_style.set_flex(0.0f, 0.0f);

	button = std::make_shared<ButtonView>();
	button->box_style.set_flex(0.0f, 0.0f);
	button->box_style.set_margin_top_auto();
	button->box_style.set_margin_bottom_auto();
	button->box_style.set_padding(10.0f, 3.0f);
	if (!icon.empty())
	{
		if (!last)
			button->image_view()->box_style.set_margin(0.0f, 0.0, 5.0f, 0.0f);
		else
			button->image_view()->box_style.set_margin(5.0f, 0.0, 0.0f, 0.0f);
		button->image_view()->set_image(ImageSource::from_resource(icon));
	}
	button->label()->set_text(StringHelp::text_to_upper(text));
	button->label()->text_style().set_font("Lato", 12, 1.4f * 13);
	button->label()->text_style().set_color(Colorf(255, 255, 255));
	slots.connect(button->sig_pointer_release(EventUIPhase::bubbling), bind_member(this, &HeaderMenuView::button_clicked));
	add_subview(button);

	if (last)
		button->move_label_before_image();

	items = std::make_shared<PopupView>();
	items->box_style.set_layout_vbox();
	items->box_style.set_absolute();
	items->box_style.set_right(0.0f);
	items->box_style.set_top(20.0f);
	items->box_style.set_width(150.0f);
	items->box_style.set_background(Colorf::gray90);
	items->set_hidden(true);
	add_subview(items);
}

void HeaderMenuView::button_clicked(PointerEvent &e)
{
	e.stop_propagation();
	items->set_hidden(!items->hidden());
}

void HeaderMenuView::add_item(const std::string &text, std::function<void()> click)
{
	auto button = std::make_shared<ButtonView>();
	button->box_style.set_flex(0.0f, 0.0f);
	button->box_style.set_padding(10.0f, 3.0f);
	button->label()->set_text(StringHelp::text_to_upper(text));
	button->label()->text_style().set_font("Lato", 12, 1.4f * 13);
	button->label()->text_style().set_color(Colorf(255, 255, 255));
	slots.connect(button->sig_pointer_release(EventUIPhase::bubbling), [=](PointerEvent &e) { click(); e.stop_propagation(); });
}
