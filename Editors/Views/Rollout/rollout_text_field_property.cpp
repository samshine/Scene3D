
#include "precomp.h"
#include "rollout_text_field_property.h"
#include "rollout_view.h"

using namespace uicore;

RolloutTextFieldProperty::RolloutTextFieldProperty(const std::string &label_text)
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

	text_field = std::make_shared<TextFieldBaseView>();
	text_field->style()->set("flex: auto");
	text_field->style()->set("background: rgba(255,255,255,0.07843)");
	text_field->style()->set("border-radius: 3px");
	text_field->style()->set("padding: 2px 5px");
	text_field->style()->set("font: 12px/18px 'Lato'");
	text_field->style()->set("color: rgb(230,230,230)");

	add_child(label);
	add_child(text_field);

	slots.connect(text_field->sig_enter_pressed(), [&, this](KeyEvent *)
	{
		_sig_value_changed();
	});
}
