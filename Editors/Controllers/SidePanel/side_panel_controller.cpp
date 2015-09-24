
#include "precomp.h"
#include "side_panel_controller.h"

using namespace uicore;

SidePanelController::SidePanelController()
{
	set_root_view(view);
	view->style()->set("flex: auto");
	view->style()->set("flex-direction: column");
	view->content_view()->style()->set("flex-direction: column");
}
