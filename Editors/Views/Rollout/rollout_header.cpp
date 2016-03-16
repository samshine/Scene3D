
#include "precomp.h"
#include "rollout_header.h"

using namespace uicore;

RolloutHeader::RolloutHeader()
{
	style()->set("flex-direction: column");
	style()->set("padding: 2px 10px");
	style()->set("background: rgb(6,31,62)");

	label = std::make_shared<LabelBaseView>();
	label->style()->set("font: 12px/18px 'Lato'");
	label->style()->set("color: white");
	add_child(label);
}

void RolloutHeader::set_text(const std::string &text)
{
	label->set_text(text);
}
