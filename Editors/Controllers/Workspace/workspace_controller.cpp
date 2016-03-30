
#include "precomp.h"
#include "workspace_controller.h"

using namespace uicore;

WorkspaceController::WorkspaceController()
{
	view->style()->set("flex-direction: row");
	view->style()->set("flex: auto");

	center_view = std::make_shared<View>();
	center_view->style()->set("flex: auto");
	center_view->style()->set("flex-direction: column");
	//center_view->style()->set("background: rgb(236,240,243)");
	//center_view->style()->set("padding: 5px");

	dock_view = std::make_shared<View>();
	dock_view->style()->set(R"(
		flex: none;
		width: 250px;
		background: rgb(83,83,83);/*rgb(47,75,99);*/
		border-left: 1px solid rgb(109,109,109);
		flex-direction: column;
		)");
	dock_view->set_hidden(true);

	view->add_child(center_view);
	view->add_child(dock_view);
}

void WorkspaceController::set_center(const std::shared_ptr<WorkspaceDockableController> &view_controller)
{
	if (center == view_controller) return;

	if (center)
	{
		center->root_view()->remove_from_parent();
		center->workspace_controller_ptr = nullptr;
		center.reset();
	}

	if (view_controller)
		center_view->add_child(view_controller->root_view());

	center = view_controller;
}

void WorkspaceController::set_docked(const std::shared_ptr<WorkspaceDockableController> &view_controller)
{
	if (docked == view_controller) return;

	if (docked)
	{
		docked->root_view()->remove_from_parent();
		docked.reset();
	}

	if (view_controller)
		dock_view->add_child(view_controller->view);

	docked = view_controller;
	dock_view->set_hidden(!docked);
}

void WorkspaceController::remove(WorkspaceDockableController *controller)
{
	if (center.get() == controller)
	{
		center->root_view()->remove_from_parent();
		center.reset();
	}
	else if (docked.get() == controller)
	{
		docked->root_view()->remove_from_parent();
		docked.reset();
	}
}
