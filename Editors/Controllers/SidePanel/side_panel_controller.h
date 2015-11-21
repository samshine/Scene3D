
#pragma once

#include "../Workspace/workspace_controller.h"
#include "Views/Theme/theme_views.h"

class SidePanelController : public WorkspaceDockableController
{
public:
	SidePanelController();

	std::shared_ptr<uicore::ScrollView> view = std::make_shared<ThemeScrollView>();
};
