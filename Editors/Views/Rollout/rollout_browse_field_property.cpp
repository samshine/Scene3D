
#include "precomp.h"
#include "rollout_browse_field_property.h"
#include "rollout_view.h"

using namespace uicore;

RolloutBrowseFieldProperty::RolloutBrowseFieldProperty(const std::string &label_text)
{
	style()->set("flex-direction: row");
	style()->set("margin: 3px 0");

	label = std::make_shared<LabelBaseView>();
	label->style()->set("margin-right: 5px");
	label->style()->set("flex: none");
	label->style()->set(string_format("width: %1px", RolloutView::label_width));
	label->style()->set("padding: 2px 0");
	label->style()->set("font: 12px/18px 'Lato'");
	label->style()->set("color: rgb(153,180,198)");
	label->set_text(label_text);

	browse_field = std::make_shared<LabelBaseView>();
	browse_field->style()->set("flex: auto");
	browse_field->style()->set("background: rgba(255,255,255,0.07843)");
	browse_field->style()->set("border-radius: 3px");
	browse_field->style()->set("padding: 2px 5px");
	browse_field->style()->set("font: 12px/18px 'Lato'");
	browse_field->style()->set("color: white");

	add_child(label);
	add_child(browse_field);

	slots.connect(browse_field->sig_pointer_release(), [&, this](PointerEvent &e)
	{
		_sig_browse();
	});
}
