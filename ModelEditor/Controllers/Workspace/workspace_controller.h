
#pragma once

class WorkspaceController : public clan::ViewController
{
public:
	WorkspaceController();

	void set_center(const std::shared_ptr<clan::ViewController> &view_controller);
	void set_docked(const std::shared_ptr<clan::ViewController> &view_controller);

protected:
	void child_controller_added(const std::shared_ptr<clan::ViewController> &view_controller) override;
	void child_controller_removed(const std::shared_ptr<clan::ViewController> &view_controller) override;

private:
	std::shared_ptr<clan::View> center_view;
	std::shared_ptr<clan::View> dock_view;
	std::shared_ptr<clan::ViewController> docked;
	std::shared_ptr<clan::ViewController> center;
};
