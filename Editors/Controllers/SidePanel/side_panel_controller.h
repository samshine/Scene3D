
#pragma once

#include "../Workspace/workspace_controller.h"

class SidePanelController : public WorkspaceDockableController
{
public:
	SidePanelController();

	std::shared_ptr<uicore::ScrollView> view = std::make_shared<uicore::ScrollView>();
};
