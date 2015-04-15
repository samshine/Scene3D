
#pragma once

#include "Controllers/SidePanel/side_panel_controller.h"

class RolloutView;
class RolloutTextFieldProperty;
class RolloutList;
class RolloutListItemView;

class ModifyController : public SidePanelController
{
public:
	ModifyController();

private:
	clan::SlotContainer slots;
};
