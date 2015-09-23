
#pragma once

class WorkspaceController : public uicore::ViewController
{
public:
	WorkspaceController();

	void set_center(const std::shared_ptr<uicore::ViewController> &view_controller);
	void set_docked(const std::shared_ptr<uicore::ViewController> &view_controller);

protected:
	void child_controller_added(const std::shared_ptr<uicore::ViewController> &view_controller) override;
	void child_controller_removed(const std::shared_ptr<uicore::ViewController> &view_controller) override;

private:
	std::shared_ptr<uicore::View> center_view;
	std::shared_ptr<uicore::View> dock_view;
	std::shared_ptr<uicore::ViewController> docked;
	std::shared_ptr<uicore::ViewController> center;
};
