
#include "precomp.h"
#include "rollout_text_field_property.h"
#include "rollout_view.h"

using namespace clan;

RolloutTextFieldProperty::RolloutTextFieldProperty(const std::string &label_text)
{
	box_style.set_layout_hbox();
	box_style.set_margin(0.0f, 3.0f);

	label = std::make_shared<LabelView>();
	label->box_style.set_margin(0.0f, 0.0f, 5.0f, 0.0f);
	label->box_style.set_flex(0.0f, 0.0f);
	label->box_style.set_width(RolloutView::label_width);
	label->box_style.set_padding(0.0f, 2.0f);
	label->text_style().set_font("Lato", 12, 1.4f * 13);
	label->text_style().set_color(Colorf(153, 180, 198));
	label->set_text(label_text);

	text_field = std::make_shared<TextFieldView>();
	text_field->box_style.set_flex(1.0f, 1.0f);
	text_field->box_style.set_background(Colorf(255, 255, 255, 20));
	text_field->box_style.set_border_radius(3.0f);
	text_field->box_style.set_padding(5.0f, 2.0f);
	text_field->text_style().set_font("Lato", 12, 1.4f * 13);
	text_field->text_style().set_color(Colorf(255, 255, 255));

	add_subview(label);
	add_subview(text_field);

	slots.connect(text_field->sig_enter_pressed(), [&, this]()
	{
		_sig_value_changed();
	});
}