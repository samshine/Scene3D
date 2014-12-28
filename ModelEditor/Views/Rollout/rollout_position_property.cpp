
#include "precomp.h"
#include "rollout_position_property.h"
#include "rollout_view.h"

using namespace clan;

RolloutPositionProperty::RolloutPositionProperty(const std::string &label_text)
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
	add_subview(label);

	auto value_group = std::make_shared<View>();
	value_group->box_style.set_layout_vbox();
	add_subview(value_group);

	char *label_texts[] = { "X:", "Y:", "Z:" };
	for (int i = 0; i < 3; i++)
	{
		auto row = std::make_shared<View>();
		row->box_style.set_layout_hbox();
		if (i > 0)
			row->box_style.set_padding(0.0f, 5.0f, 0.0f, 0.0f);

		std::shared_ptr<LabelView> input_label = std::make_shared<LabelView>();
		input_label->box_style.set_margin(0.0f, 0.0f, 5.0f, 0.0f);
		input_label->box_style.set_flex(0.0f, 0.0f);
		input_label->box_style.set_padding(0.0f, 2.0f);
		input_label->text_style().set_font("Lato", 12, 1.4f * 13);
		input_label->text_style().set_color(Colorf(153, 180, 198));
		input_label->set_text(label_texts[i]);
		row->add_subview(input_label);

		std::shared_ptr<TextFieldView> input = std::make_shared<TextFieldView>();
		input->box_style.set_flex(1.0f, 1.0f);
		input->box_style.set_background(Colorf(255, 255, 255, 20));
		input->box_style.set_border_radius(3.0f);
		input->box_style.set_padding(2.0f, 2.0f);
		input->text_style().set_font("Lato", 12, 1.4f * 13);
		input->text_style().set_color(Colorf(255, 255, 255));
		row->add_subview(input);

		value_group->add_subview(row);

		slots.connect(input->sig_enter_pressed(), [&, this]()
		{
			_sig_value_changed();
		});

		switch (i)
		{
		case 0: label_x = input_label; input_x = input; break;
		case 1: label_y = input_label; input_y = input; break;
		case 2: label_z = input_label; input_z = input; break;
		}
	}
}
