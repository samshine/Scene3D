
#include "precomp.h"
#include "side_panel_controller.h"

using namespace uicore;

SidePanelController::SidePanelController()
{
	view = std::make_shared<ScrollView>();
	view->style()->set("flex: auto");
	view->style()->set("flex-direction: column");
	content_view()->style()->set("flex-direction: column");
}

std::shared_ptr<View> SidePanelController::content_view() const
{
	return std::static_pointer_cast<ScrollView>(view)->content_view();
}
