
#include "precomp.h"
#include "rollout_position_property.h"
#include "rollout_view.h"

using namespace uicore;

RolloutPositionProperty::RolloutPositionProperty(const std::string &label_text)
{
	style()->set("flex-direction: row");
	style()->set("margin: 3px 0");

	label = std::make_shared<LabelBaseView>();
	label->style()->set("margin-right: 5px");
	label->style()->set("flex: none");
	label->style()->set(string_format("width: %1px", RolloutView::label_width));
	label->style()->set("padding: 2px 0");
	label->style()->set("font: 12px/18px 'Lato'");
	label->style()->set("color: rgb(230,230,230)");
	label->set_text(label_text);
	add_child(label);

	auto value_group = std::make_shared<View>();
	value_group->style()->set("flex: auto");
	value_group->style()->set("flex-direction: column");
	add_child(value_group);

	char *label_texts[] = { "X:", "Y:", "Z:" };
	for (int i = 0; i < 3; i++)
	{
		auto row = std::make_shared<View>();
		row->style()->set("flex-direction: row");
		if (i > 0)
			row->style()->set("padding-top: 5px");

		std::shared_ptr<LabelBaseView> input_label = std::make_shared<LabelBaseView>();
		input_label->style()->set("margin-right: 5px");
		input_label->style()->set("flex: none");
		input_label->style()->set("padding: 2px 0");
		input_label->style()->set("font: 12px/18px 'Lato'");
		input_label->style()->set("color: rgb(230,230,230)");
		input_label->set_text(label_texts[i]);
		row->add_child(input_label);

		std::shared_ptr<TextFieldBaseView> input = std::make_shared<TextFieldBaseView>();
		input->style()->set("flex: auto");
		input->style()->set("background: rgba(255,255,255,0.07843)");
		input->style()->set("border-radius: 3px");
		input->style()->set("padding: 2px");
		input->style()->set("font: 12px/18px 'Lato'");
		input->style()->set("color: rgb(230,230,230)");
		row->add_child(input);

		value_group->add_child(row);

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
