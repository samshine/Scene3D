
#include "precomp.h"
#include "workspace_controller.h"

using namespace clan;

WorkspaceController::WorkspaceController()
{
	view->box_style.set_layout_hbox();
	view->box_style.set_flex(1.0f, 0.0f);

	center_view = std::make_shared<View>();
	center_view->box_style.set_layout_vbox();
	center_view->box_style.set_background(Colorf(236, 240, 243));
	//center_view->box_style.set_padding(5.0f);

	dock_view = std::make_shared<View>();
	dock_view->box_style.set_layout_vbox();
	dock_view->box_style.set_width(250.0f);
	dock_view->box_style.set_flex(0.0f, 0.0f);
	dock_view->box_style.set_background(Colorf(47, 75, 99));
	dock_view->box_style.set_border(Colorf(109, 109, 109), 1.0f, 0.0f, 0.0f, 0.0f);
	dock_view->set_hidden(true);

	view->add_subview(center_view);
	view->add_subview(dock_view);
}

void WorkspaceController::set_center(const std::shared_ptr<clan::ViewController> &view_controller)
{
	if (center == view_controller) return;

	if (center)
	{
		center->remove_from_parent_controller();
		center.reset();
	}

	if (view_controller)
	{
		add_child_controller(view_controller);
		center_view->add_subview(view_controller->view);
	}

	center = view_controller;
}

void WorkspaceController::set_docked(const std::shared_ptr<clan::ViewController> &view_controller)
{
	if (docked == view_controller) return;

	if (docked)
	{
		docked->remove_from_parent_controller();
		docked.reset();
	}

	if (view_controller)
	{
		add_child_controller(view_controller);
		dock_view->add_subview(view_controller->view);
	}

	docked = view_controller;
	dock_view->set_hidden(!docked);
}

void WorkspaceController::child_controller_added(const std::shared_ptr<clan::ViewController> &view_controller)
{
}

void WorkspaceController::child_controller_removed(const std::shared_ptr<clan::ViewController> &view_controller)
{
	if (docked == view_controller)
	{
		docked.reset();
		dock_view->set_hidden(true);
	}
	if (center == view_controller) center.reset();
	view_controller->view->remove_from_super();
}
