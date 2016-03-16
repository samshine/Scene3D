
#include "precomp.h"
#include "rollout_button.h"

using namespace uicore;

RolloutButton::RolloutButton(const std::string &label)
{
	style()->set("flex-direction: column; flex: none");
	style()->set("margin: 3px 0");

	button = std::make_shared<ButtonBaseView>();
	button->style()->set("flex: auto");
	button->style()->set("background: rgba(255,255,255,0.07843)");
	button->style()->set("border-radius: 3px");
	button->style()->set("padding: 2px 5px");
	button->label()->style()->set("flex: auto");
	button->label()->style()->set("font: 12px/18px 'Lato'");
	button->label()->style()->set("color: white");
	button->label()->set_text_alignment(TextAlignment::center);
	button->label()->set_text(label);

	button->style("selected")->set("background: rgba(255,255,255,0.27843)");

	add_child(button);
}
