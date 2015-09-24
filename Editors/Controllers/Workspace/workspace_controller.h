
#pragma once

class WorkspaceDockableController;

class WorkspaceController
{
public:
	WorkspaceController();

	std::shared_ptr<uicore::View> view = std::make_shared<uicore::View>();

	void set_center(const std::shared_ptr<WorkspaceDockableController> &view_controller);
	void set_docked(const std::shared_ptr<WorkspaceDockableController> &view_controller);

private:
	void remove(WorkspaceDockableController *controller);

	std::shared_ptr<uicore::View> center_view;
	std::shared_ptr<uicore::View> dock_view;
	std::shared_ptr<WorkspaceDockableController> docked;
	std::shared_ptr<WorkspaceDockableController> center;

	friend class WorkspaceDockableController;
};

class WorkspaceDockableController
{
public:
	const std::shared_ptr<uicore::View> &root_view() const { return view; }
	void set_root_view(const std::shared_ptr<uicore::View> &v) { view = v; }

	WorkspaceController *workspace_controller() { return workspace_controller_ptr; }
	void remove_from_workspace() { if (workspace_controller()) workspace_controller()->remove(this); }

private:
	WorkspaceController *workspace_controller_ptr = nullptr;
	std::shared_ptr<uicore::View> view = std::make_shared<uicore::View>();

	friend class WorkspaceController;
};
