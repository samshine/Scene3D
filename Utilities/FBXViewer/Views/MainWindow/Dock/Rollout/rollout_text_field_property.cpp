
#include "precomp.h"
#include "rollout_text_field_property.h"
#include "rollout_view.h"

using namespace clan;

RolloutTextFieldProperty::RolloutTextFieldProperty(const std::string &label_text)
{
	style.set_layout_hbox();
	style.set_margin(0.0f, 3.0f);

	FontDescription font_desc("Lato");
	font_desc.set_height(12);
	font_desc.set_line_height(1.4f * 13);

	label = std::make_shared<LabelView>();
	label->style.set_margin(0.0f, 0.0f, 5.0f, 0.0f);
	label->style.set_flex(0.0f, 0.0f);
	label->style.set_width(RolloutView::label_width);
	label->style.set_padding(0.0f, 2.0f);
	label->set_font(Font::resource(UIThread::get_resource_canvas(), font_desc, UIThread::get_resources()));
	label->set_text(label_text);
	label->set_text_color(Colorf(153, 180, 198));

	text_field = std::make_shared<TextFieldView>();
	text_field->style.set_flex(1.0f, 1.0f);
	text_field->style.set_background(Colorf(255, 255, 255, 20));
	text_field->style.set_border_radius(3.0f);
	text_field->style.set_padding(5.0f, 2.0f);
	text_field->set_font(Font::resource(UIThread::get_resource_canvas(), font_desc, UIThread::get_resources()));
	text_field->set_text_color(Colorf(255, 255, 255));

	add_subview(label);
	add_subview(text_field);

	slots.connect(text_field->sig_enter_pressed(), [&, this]()
	{
		_sig_value_changed();
	});
}
