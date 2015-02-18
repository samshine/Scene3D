
#include "precomp.h"
#include "rollout_header.h"

using namespace clan;

RolloutHeader::RolloutHeader()
{
	style()->set("flex-direction: row");
	style()->set("padding: 2px 10px");
	style()->set("background: rgb(6,31,62)");

	label = std::make_shared<LabelView>();
	label->style()->set("font: 12px/18px 'Lato'");
	label->style()->set("color: white");
	add_subview(label);
}

void RolloutHeader::set_text(const std::string &text)
{
	label->set_text(text);
}
